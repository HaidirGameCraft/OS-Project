#include "fat.h"
#include <driver/disk.h>
#include <memory.h>
#include <string.h>
#include <stdio.h>

union fat_compact fheader;
u32 lba_part_start = 0;
u32 typeFAT = 0;

u32 CheckFAT(partition_table_t* partt, union fat_compact* fhdr) {
    u32 fat_sector = partt->lba_start + fhdr->fat_16.header.reserved_sector;
    u32 data_sector = fat_sector + fhdr->fat_16.header.fat_number * fhdr->fat_16.header.sector_per_fat + (fhdr->fat_16.header.root_dir_entries * sizeof( struct fat_directory_t )) / fheader.fat_16.header.bytes_per_sector;
    if( fheader.fat_16.header.sector_per_fat == 0 )
    {
        // use Sector Per FAT32
        data_sector = fat_sector + fhdr->fat_32.header.fat_number * fhdr->fat_32.boot_record.sector_per_fat + (fhdr->fat_16.header.root_dir_entries * sizeof( struct fat_directory_t )) / fheader.fat_16.header.bytes_per_sector;
    }

    u32 sector_cluster = ( fhdr->fat_16.header.large_sectors - data_sector ) / fhdr->fat_16.header.sector_per_cluster;
    if ( fheader.fat_16.header.sector_per_fat != 0 )
        return FAT16;
    else
        return FAT32;
}

void                  OpenFATVolume(partition_table_t* partt) {
    lba_part_start = partt->lba_start;
    ATA_ReadDisk( lba_part_start + 0, ATA_PrimaryBus, (char*) &fheader, sizeof( union fat_compact ));
    
    typeFAT = CheckFAT(partt, &fheader);
    printf("Type: FAT%x\n", typeFAT);
    printf("Partition Start: %x\n", lba_part_start);
}

struct fat_directory_t* GetFATFile(const char* filename) {
    struct fat_directory_t* dir = (struct fat_directory_t*) malloc( sizeof( struct fat_directory_t ) );
    zeromem(dir, sizeof( struct fat_directory_t ) );

    u32 fat_sector = lba_part_start + fheader.fat_16.header.reserved_sector;
    u32 data_sector = fat_sector + fheader.fat_16.header.fat_number * fheader.fat_16.header.sector_per_fat;
    if( typeFAT == FAT12 || typeFAT == FAT16 )
    {
        uint8_t* buffer = (uint8_t*) malloc( 512 );
        for(int i = 0; i < fheader.fat_16.header.sector_per_cluster; i++ )
        {
            ATA_ReadDisk(data_sector + i, ATA_PrimaryBus, buffer, 512 );
            int j = 0;
            while( j < ( fheader.fat_16.header.bytes_per_sector / sizeof( struct fat_directory_t ) ) ) {
                
                struct fat_directory_t* d = &((struct fat_directory_t*) buffer)[j];
                if( strncmp(d->filename, filename, 11) )
                {
                    memcpy(dir, d, sizeof( struct fat_directory_t ) );
                    printf("[DEBUG] FAT%x, %s Found\n", typeFAT, filename);
                    free( buffer );
                    return dir;
                }
                j++;
            }
        }
        free( buffer );

        // Error
        printf("%s File Not Found\n", filename);
    }
    else {
        uint8_t* buffer = (uint8_t*) malloc( 512 );
        u32 cluster = fheader.fat_32.boot_record.cluster;
        data_sector = fat_sector + fheader.fat_16.header.fat_number * fheader.fat_32.boot_record.sector_per_fat;
        while( cluster < 0x0FFFFFF0 )
        {
            u32 dir_sector = data_sector + (cluster - 2) * fheader.fat_32.header.sector_per_cluster;
            printf("%x\n", dir_sector);
            ATA_ReadDisk(dir_sector, ATA_PrimaryBus, buffer, 512 );
            int j = 0;
            while( j < ( fheader.fat_32.header.bytes_per_sector / sizeof( struct fat_directory_t ) ) ) {
                
                struct fat_directory_t* d = &((struct fat_directory_t*) buffer)[j];
                if( strncmp(d->filename, filename, 11) )
                {
                    memcpy(dir, d, sizeof( struct fat_directory_t ) );
                    printf("[DEBUG] FAT%x, %s Found\n", typeFAT, filename);
                    free( buffer );
                    return dir;
                }
                j++;
            }

            u32 fat_section = (int)( cluster / 128 );
            u32 fat_offset = (int)( cluster % 128 );
            ATA_ReadDisk( fat_sector + fat_section, ATA_PrimaryBus, buffer, 512 );
            cluster = ((u32*) buffer)[fat_offset];
        }
        free( buffer );

        // Error
        printf("%s File Not Found\n", filename);
    }
}

void                    ReadFATFile(struct fat_directory_t* dir, char* buffer, size_t size ) {
    u32 cluster = (dir->high_cluster << 16) | dir->low_cluster;
    u32 fat_sector = lba_part_start + fheader.fat_16.header.reserved_sector;
    u32 data_sector = fat_sector + fheader.fat_16.header.fat_number * fheader.fat_16.header.sector_per_fat;
    if( typeFAT == FAT12 || typeFAT == FAT16 )
    {
        data_sector += (fheader.fat_16.header.root_dir_entries * sizeof( struct fat_directory_t )) / fheader.fat_16.header.bytes_per_sector;
        uint8_t* _tmp = (uint8_t*) malloc( 512 );
        while( cluster < 0x0FF0 )
        {
            u32 data_section = data_sector + ( cluster - 2 ) * fheader.fat_16.header.sector_per_cluster;
            if( ((int) size / ( 512 * fheader.fat_16.header.sector_per_cluster ) ) > 0 )
            {
                ATA_ReadDisk(data_section, ATA_PrimaryBus, buffer, 512 * fheader.fat_16.header.sector_per_cluster );
                buffer += fheader.fat_16.header.sector_per_cluster * 512;
            }
            else {
                ATA_ReadDisk(data_section, ATA_PrimaryBus, buffer, size % (512 * fheader.fat_16.header.sector_per_cluster) );
            }

            u32 fat_section = (int)( cluster / 256 );
            u32 fat_offset = (int)( cluster % 256 );
            ATA_ReadDisk( fat_sector + fat_section, ATA_PrimaryBus, _tmp, 512 );
            cluster = ((u16*)_tmp)[fat_offset];            
        }
    }
    else {

    }
    
}