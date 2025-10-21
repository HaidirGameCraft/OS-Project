#ifndef __MBR_DISK__
#define __MBR_DISK__

/**
 * Master Boot Record (MBR) Format
 * Based On: https://wiki.osdev.org/MBR_(x86)
 */


#include <stdint.h>

typedef struct {
    u8  attribute;
    u8  chs_start[3];
    u8  type;
    u8  chs_end[3];
    u32 lba_start;
    u32 sector_part;        // the total sector of partiton
} __attribute__((packed)) partition_table_t;

typedef struct {
    u8  bootcode[440]; // containing Flat Binary executable code
    u32 signature;
    u16 reserved;               // always zero
    partition_table_t partition_tables[4];
    u16 valid_bootrecord;       // always 0x55AA
} __attribute__((packed)) mbr_header_t;


/**
 * Partition Table Type
 * Based On: http://www.osdever.net/documents/partitiontypes.php
 */
#define PartType_NONE                   0x00
#define PartType_FAT12                  0x01
#define PartType_FAT16_Lower            0x04
#define PartType_Extended_Part          0x05
#define PartType_FAT16_High             0x06
#define PartType_FAT32_CHS              0x0B
#define PartType_FAT32_LBA              0x0C

#define PartType_Hidden_FAT16_Lower     0x14
#define PartType_Hidden_FAT16_High      0x16
#define PartType_Hidden_FAT32_CHS       0x1B
#define PartType_Hidden_FAT32_LBA       0x1C

/**
 * OpenDisk
 * - Initialize the Primary Disk
 */
void OpenMBRDisk();

/**
 * OpenMBRVolume
 * - Get the partiton that available on MBR Header Disk
 * @return partition table pointer
 */
partition_table_t* OpenMBRPart(int part);

/**
 * GetFileVolume()
 * - Get the File Volume
 * @return type of FileSystem
 */
void*   GetFileVolume(partition_table_t* partt);


/**
 * CloseDisk()
 * - Closing all attachment from Primary Disk
 */
void CloseDisk();

#endif