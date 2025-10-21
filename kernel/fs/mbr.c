#include <stdint.h>
#include "mbr.h"

#include <stdio.h>
#include <driver/disk.h>
#include <string.h>
#include <memory.h>


mbr_header_t mbr_header;

/**
 * OpenDisk
 * - Initialize the Primary Disk
 */
void OpenMBRDisk()
{
    zeromem(&mbr_header, sizeof( mbr_header_t ) );
    ATA_ReadDisk(0, ATA_PrimaryBus, (char*) &mbr_header, sizeof( mbr_header_t ) );
}

/**
 * OpenMBRVolume
 * - Get the partiton that available on MBR Header Disk
 * @return partition table pointer
 */
partition_table_t* OpenMBRPart(int part) {
    if( part < 0 || part >= 4 )
    {
        printf("Volume is Out Of Range (0 - 3)\n");
        return NULL;
    }

    return &mbr_header.partition_tables[ part ];
}

/**
 * GetFileVolume()
 * - Get the File Volume
 * @return type of FileSystem
 */
void*   GetFileVolume(partition_table_t* partt) {
    u8 p = partt->type;

    if( p == PartType_FAT16_Lower || p == PartType_Hidden_FAT16_Lower )
    {
        //struct fat_header* fhdr = (struct fat_header*)
    }
    else if ( p == PartType_FAT16_High || p == PartType_Hidden_FAT16_High )
    {

    }
    else if ( p == PartType_FAT32_CHS || p == PartType_FAT32_LBA || p == PartType_Hidden_FAT32_CHS || p == PartType_Hidden_FAT32_LBA )
    {

    }
    else {
        return NULL;
    }
}


/**
 * CloseDisk()
 * - Closing all attachment from Primary Disk
 */
void CloseDisk();