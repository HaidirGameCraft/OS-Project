#include "../include/stdint.h"
#include "../include/memory.h"
#include "../include/fs.h"
#include "../include/cpu.h"
#include "../include/bootlib.h"
#include "../include/string.h"

void ReadDisk( u32 lba, char* buffer ) {
    oportb(PRIMARY_ATA + ATA_DRIVE, ATA_MASTER | (( lba >> 24 ) & 0x0F ) );

    oportb(PRIMARY_ATA + ATA_SECTOR_COUNT, 0x01);
    oportb(PRIMARY_ATA + ATA_LBA_LOW, (u8) lba & 0xFF );
    oportb(PRIMARY_ATA + ATA_LBA_MID, (u8) ( lba >> 8 ) & 0xFF );
    oportb(PRIMARY_ATA + ATA_LBA_HIGH, (u8) (lba >> 16) & 0xFF );
    oportb(PRIMARY_ATA + ATA_CMD_REGISTER, READ_DISK_CMD);
    
    while( (iportb( PRIMARY_ATA + ATA_STATUS_REGISTER ) & 0x80) );
    
    for(int i = 0; i < 256; i++)
    {
        ((u16*) buffer)[i] = iportw( 0x1F0 );
    }
}

void WriteDisk( u32 lba, char* buffer ) {
    oportb(PRIMARY_ATA + ATA_DRIVE, ATA_MASTER | (( lba >> 24 ) & 0x0F ) );

    oportb(PRIMARY_ATA + ATA_SECTOR_COUNT, 0x01);
    oportb(PRIMARY_ATA + ATA_LBA_LOW, lba & 0xFF );
    oportb(PRIMARY_ATA + ATA_LBA_MID, ( lba >> 8 ) & 0xFF );
    oportb(PRIMARY_ATA + ATA_LBA_HIGH, (lba >> 16) & 0xFF );
    oportb(PRIMARY_ATA + ATA_CMD_REGISTER, WRITE_DISK_CMD);

    while( iportb( PRIMARY_ATA + ATA_STATUS_REGISTER ) & 0x80 );

    for(int i = 0; i < 256; i++)
    {
        oportw( PRIMARY_ATA + 0, ((u16*) buffer)[i] );
    }
}

BootDiskInfo GetDiskInfo() {
    BootDiskInfo bdiskinfo = { 0 };

    MBR_Header* header = (MBR_Header*) malloc( sizeof( MBR_Header ) );
    ReadDisk(0, (char*) header);

    bdiskinfo.type = TypeDisk_MBR;
    bdiskinfo.ptr = (void*) header;


    return bdiskinfo;
}

VolumeInfo OpenVolume(BootDiskInfo* bdiskinfo, int partition_number) {
    VolumeInfo volume_info = { 0 };
    if( bdiskinfo->type == TypeDisk_MBR )
    {
        if( partition_number <= 0 || partition_number > 4 )
        {
            printf("OpenVolume Error \"%s\": The partition selected is over or out of range of MBR\n", __FILE__);
            return volume_info;
        }

        MBR_Header* header = (MBR_Header*) bdiskinfo->ptr;
        Partition_Table* parts = &header->partitions[ partition_number -1 ];


        FATHeaderCompact *fat_header = (FATHeaderCompact*) malloc( sizeof( FATHeaderCompact ) );
        char* buffer = (char*) malloc( 0x200 );

        ReadDisk( parts->lba_start, buffer );
        memcpy( &fat_header->header, buffer, sizeof( FATHeader ) );

        if( fat_header->header.large_sector < 2048 )
            volume_info.type = VolumeType_Fat12;
        else if ( fat_header->header.large_sector < 65536 )
            volume_info.type = VolumeType_Fat16;
        else
            volume_info.type = VolumeType_Fat32;

        
        if( volume_info.type == VolumeType_Fat32 )
        {
            memcpy(&fat_header->extended, buffer + sizeof( FATHeader ), sizeof( FAT32ExtendedBootRecord ) + sizeof( FATBootRecord ) );
        }
        else {
            memcpy(&fat_header->boot_record, buffer + sizeof( FATHeader ), sizeof( FATBootRecord ) );
        }


        volume_info.partition = parts;
        volume_info.volume_header = fat_header;

        free( buffer );
    }

    return volume_info;
}

void         FAT_ReadFile(VolumeInfo* volumeInfo, FATDirectory* directory, char* buffer ) {
    int lba_start = volumeInfo->partition->lba_start;
    // Check if it is FAT Format File
    if( volumeInfo->type != VolumeType_Fat32 && volumeInfo->type != VolumeType_Fat16 && volumeInfo->type != VolumeType_Fat12 )
    {
        printf("FAT_FindFile (Error): This Volume is not FAT Format\n");
        return;
    }

    FATHeaderCompact* headerCompact = (FATHeaderCompact*) volumeInfo->volume_header;
    FATHeader* header = &headerCompact->header;

    u32 fatStart = lba_start + header->reserved_sectors;
    u32 cluster = directory->high_cluster << 16 | directory->low_cluster;
    if( volumeInfo->type == VolumeType_Fat32 )
    {

    }
    else {
        u32 data_start = fatStart + header->sectors_per_fat * header->fat_number + header->root_dir_entries * sizeof( FATDirectory ) / header->bytes_per_sectors;
        u32 entries_per_sector = header->bytes_per_sectors / sizeof( FATDirectory );
        char* fat_entries = (char*) malloc( header->bytes_per_sectors );
        while( cluster < 0xFFF0 )
        {
            printf("Cluster: %x\n", cluster);
            u32 cluster_seg = data_start + header->sectors_per_cluster * ( cluster - 2 );
            for(int i = 0; i < header->sectors_per_cluster; i++)
            {
                ReadDisk( cluster_seg + i, buffer );
                buffer += header->bytes_per_sectors;
            }

            u32 fat_per_sector = header->bytes_per_sectors / 2;
            u32 fat_index = (u32)( cluster / ( fat_per_sector ) );
            u16 fat_offset = cluster % fat_per_sector;
            printf("FAT (Index, Offset): (%x, %x)\n", fat_index, fat_offset);
            ReadDisk(fatStart + fat_index, fat_entries );
            cluster = ((u16*) fat_entries)[ fat_offset ];
        }
    }
}







FATDirectory FAT_FindFile(VolumeInfo* volumeinfo, const char* filename ) {
    FATDirectory result = {0};

    // Check if it is FAT Format File
    if( volumeinfo->type != VolumeType_Fat32 && volumeinfo->type != VolumeType_Fat16 && volumeinfo->type != VolumeType_Fat12 )
    {
        printf("FAT_FindFile (Error): This Volume is not FAT Format\n");
        return result;
    }

    FATHeaderCompact* header_compact = (FATHeaderCompact*) volumeinfo->volume_header;

    u32 fat_start = volumeinfo->partition->lba_start + header_compact->header.reserved_sectors;
    u32 data_start = 0;
    u32 cluster = 0;
    u32 end_of_file = 0;

    if( volumeinfo->type == VolumeType_Fat32 )
    {
        data_start = fat_start + header_compact->extended.sectors_per_fat * header_compact->header.fat_number;
        cluster = header_compact->extended.cluster_number;
        end_of_file = 0xFFFFFFF8;

        while( cluster < end_of_file )
        {
            u32 cluster_seg = data_start + header_compact->header.sectors_per_cluster * ( cluster - 2 );
            
        }
    }
    else
    {
        u32 root_dir_start = fat_start + header_compact->header.sectors_per_fat * header_compact->header.fat_number;
        data_start = root_dir_start + header_compact->header.root_dir_entries * sizeof( FATDirectory ) / header_compact->header.bytes_per_sectors;
        cluster = 0;
        end_of_file = 0xFFF8;

        u32 sector_per_root_dir = header_compact->header.root_dir_entries * sizeof( FATDirectory ) / header_compact->header.bytes_per_sectors;
        FATDirectory* dirs = (FATDirectory*) malloc( 0x200 );
        
        for(int i = 0; i < sector_per_root_dir; i++) {
            ReadDisk( root_dir_start + i, (char*) dirs );
            for(int j = 0; j < 16; j++ ) {
                FATDirectory* dir = &dirs[j];
                if( strncmp( dir->filename, filename, 11 ) )
                {
                    memcpy( &result, dir, sizeof( FATDirectory ) );
                    free( dirs );
                    return result;
                }
            }
        }


        printf("FATFindFile (ERROR): %s is not Found\n", filename);
        free( dirs );
        return result;
    }
}