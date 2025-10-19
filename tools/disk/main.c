#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "fs.h"

FILE* OpenDisk(const char* file);
FILE* CreateDisk(const char* filename, uint32_t sectors, uint32_t bytes_per_sector);
void  CloseDisk(FILE* file);

// MBR
struct mbr_struct_t* CreateMBR(FILE* file);
struct mbr_struct_t* ReadMBR(FILE* file);
void                 WriteMBR(FILE* file, struct mbr_struct_t* mbr_struct);

// FAT
void CreateFAT16(FILE* file, long large_sector, long lba_start, FILE* data);


int arg_compare(const char** argv, const char* text, int argc) {
    for(int i = 0; i < argc; i++) {
        if( strcmp(argv[i], text) == 0 )
            return 1;
    }

    return 0;
}

const char* arg_get(const char** argv, int argc, const char* text)
{
    for(int i = 0; i < argc; i++) {
        if( strcmp(argv[i], text) == 0 )
            return argv[i];
    }

    return NULL;
}

const char* arg_get_next(const char** argv, const char* text, int argc) {
    for(int i = 0; i < argc; i++) {
        if( strncmp(argv[i], text, strlen( text ) ) == 0 )
            return argv[i + 1];
    }

    return NULL;
}

/**
 * COMMAND:
 * disk --create --output <file name>
 * disk --open <file name> --part boot <disk type> -bs <value> -st <value> -et <value> --data <file name>
 */


int main(int argc, const char* argv[]) {
    
    FILE* disk = NULL;
    struct mbr_struct_t* mbr_struct = NULL;
    struct fat_header* fat_header = NULL;
    struct fat_extended_boot_record* febr = NULL;
    struct fat32_extended_boot_record* f32ebr = NULL;

    // when the disk is not define or file not found
    if( arg_compare(argv, "--create", argc) )
    {
        disk = CreateDisk(arg_get_next(argv, "--output", argc), 
                            (uint32_t) atoi(arg_get_next(argv, "--count", argc)), 
                            (uint32_t) atoi(arg_get_next(argv, "--bs", argc)));
        mbr_struct = CreateMBR(disk);
    }
    else if ( arg_compare(argv, "--part", argc ) )
    {
        disk = OpenDisk(arg_get_next(argv, "--open", argc));
        fseek(disk, 0, SEEK_END);
        long filesize = ftell(disk);
        fseek(disk, 0, SEEK_SET);

        mbr_struct = ReadMBR(disk);
        struct partition_table_t* part = NULL;
        struct partition_table_t zero = {0};

        int index = 0;
        uint32_t total_sector_inavailable = 0;
        while( index < 4 )
        {
            if( memcmp( &zero, &mbr_struct->partition[index], sizeof( struct partition_table_t ) ) == 0 )
            {
                part = &mbr_struct->partition[index];

                if( index + 1 < 4) {
                    total_sector_inavailable += mbr_struct->partition[index + 1].sector_partition;
                }
                break;
            }
            total_sector_inavailable += mbr_struct->partition[index].sector_partition;
            index++;

            if( index == 4 )
            {
                perror("MBR Volume: It not Available");
                free( mbr_struct );
                CloseDisk( disk );
                return -1;
            }
        }

        uint8_t boot_active = 0;
        if( arg_compare(argv, "boot", argc) )
            boot_active = 1;

        FILE* data = fopen(arg_get_next(argv, "--data", argc), "rb+");
        if( data == NULL ) {
            printf("ERROR: (%s) Data file is not found\n", arg_get_next(argv, "--data", argc));
            free( mbr_struct );
            CloseDisk( disk );
            return -1;
        }

        if( arg_compare(argv, "fat16", argc) )
        {
            int  bps = atoi( arg_get_next(argv, "-bs", argc) );
            long start_addr = atol(arg_get_next(argv, "-st", argc)) * bps;
            long end_addr = atol(arg_get_next(argv, "-et", argc)) * bps;

            long length = (int)((end_addr - start_addr) / 512);
            long lba_start = (int)(start_addr / 512);
            long lba_end = (int)(end_addr / 512);

            part->lba_start = lba_start;
            part->sector_partition = length;
            if( boot_active )
                part->attributes = 1 << 7;
            // based on https://thestarman.pcministry.com/asm/mbr/PartTypes.htm
            if( length < 65536)
                part->part_type = 0x04;
            else 
                part->part_type = 0x06;

            printf("Disk Sector Available: %x\n", (int)(filesize / 512) - total_sector_inavailable - 1);
            if( (int)(filesize / 512) - total_sector_inavailable - 1 <= 0 )
            {
                perror("ERROR: Disk is out of sector");
                free( mbr_struct );
                CloseDisk( disk );
                return -1;
            }

            fseek(disk, lba_start * 512, SEEK_SET);
            for(int i = 0; i < length; i++) 
            {
                uint8_t buffer[512];
                fread(buffer, 512, 1, data);
                fwrite(buffer, 512, 1, disk);
            }

            WriteMBR(disk, mbr_struct);
        }
        else if( arg_compare(argv, "fat32", argc) )
        {
            int  bps = atoi( arg_get_next(argv, "-bs", argc) );
            long start_addr = atol(arg_get_next(argv, "-st", argc)) * bps;
            long end_addr = atol(arg_get_next(argv, "-et", argc)) * bps;

            long length = (int)((end_addr - start_addr) / 512);
            long lba_start = (int)(start_addr / 512);
            long lba_end = (int)(end_addr / 512);

            part->lba_start = lba_start;
            part->sector_partition = length;
            if( boot_active )
                part->attributes = 1 << 7;
            // based on https://thestarman.pcministry.com/asm/mbr/PartTypes.htm
            part->part_type = 0x0C;

            printf("Disk Sector Available: %x\n", (int)(filesize / 512) - total_sector_inavailable - 1);
            if( (int)(filesize / 512) - total_sector_inavailable - 1 <= 0 )
            {
                perror("ERROR: Disk is out of sector");
                free( mbr_struct );
                CloseDisk( disk );
                return -1;
            }


            fseek(disk, lba_start * 512, SEEK_SET);
            for(int i = 0; i < length; i++) 
            {
                uint8_t buffer[512];
                fread(buffer, 512, 1, data);
                fwrite(buffer, 512, 1, disk);
            }

            WriteMBR(disk, mbr_struct);
        }

        fclose( data );
        
    }

    
    free( mbr_struct );
    CloseDisk( disk );

    return 0;
}


FILE* OpenDisk(const char* file) {
    FILE* disk = fopen(file, "rwb+");
    
    return disk;
}

FILE* CreateDisk(const char* filename, uint32_t sectors, uint32_t bytes_per_sector) {
    printf("Sectors: %x, Bytes Per Sector: %x\n", sectors, bytes_per_sector);
    FILE* file = fopen( filename, "wb+");
    assert(file != NULL);
    
    char zero = 0;
    for(int i = 0; i < sectors * bytes_per_sector; i++)
    {
        fwrite(&zero, 1, 1, file);
    }

    return file;
}

void  CloseDisk(FILE* file) {
    fclose( file );
}

struct mbr_struct_t*  CreateMBR(FILE* file) {
    struct mbr_struct_t* mbr_struct = (struct mbr_struct_t*) malloc( sizeof( struct mbr_struct_t ) );
    memset(mbr_struct, 0, sizeof( struct mbr_struct_t ) );

    mbr_struct->last_signature = 0xAA55;
    fseek(file, 0, SEEK_SET);
    fwrite( mbr_struct, sizeof( struct mbr_struct_t ), 1, file);
    return mbr_struct;
}

struct mbr_struct_t* ReadMBR(FILE* file) {
    struct mbr_struct_t* mbr_struct = (struct mbr_struct_t*) malloc( sizeof( struct mbr_struct_t ) );
    memset(mbr_struct, 0, sizeof( struct mbr_struct_t ) );

    fseek(file, 0, SEEK_SET);
    fread(mbr_struct, sizeof( struct mbr_struct_t ), 1, file );
    return mbr_struct;
}
void WriteMBR(FILE* file, struct mbr_struct_t* mbr_struct) {
    fseek(file, 0, SEEK_SET);
    fwrite(mbr_struct, sizeof( struct mbr_struct_t ), 1, file );
}

void CreateFAT16(FILE* file, long large_sector, long lba_start, FILE* data) {
    fseek(file, 0, SEEK_END);
    long filesize = ftell(file);
    fseek(file, 0, SEEK_SET);

    long sector_remained = (int)( filesize / 512 ) - lba_start;

}