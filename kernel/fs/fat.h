#ifndef __FAT__
#define __FAT__

/**
 * FileSystem - File Allocator Table (FAT)
 * Creator: Microsoft
 * Based On: https://wiki.osdev.org/FAT#Overview
 */

#include "mbr.h"
#include <stdint.h>

#define FAT_BOOTCODE_SIGNATURE      0x55AA
#define FAT_READONLY                0x01
#define FAT_HIDDEN                  0x02
#define FAT_SYSTEM                  0x04
#define FAT_VOLUME_ID               0x08
#define FAT_DIRECTORY               0x10
#define FAT_ARCHIVE                 0X20
#define FAT_LONGFILENAME            0x0F

#define FAT12                       0x12
#define FAT16                       0x16
#define FAT32                       0x32

struct fat_header {
    u8  jmp_code[3];
    u8  oem_identifier[8];
    u16 bytes_per_sector;
    u8  sector_per_cluster;
    u16 reserved_sector;
    u8  fat_number;
    u16 root_dir_entries;
    u16 total_sector;
    u8  media_type;
    u16 sector_per_fat;
    u16 sector_per_track;
    u16 heads;
    u32 hidden_sectors;
    u32 large_sectors;
} __attribute__((packed));

struct fat_extended_boot_record {
    u8  drive_number;
    u8  reserved;
    u8  signature;
    u32 volume_id;
    u8  volume_label[11];
    u8  system_identifier[8];
    u8  bootcode[448];
    u16 part_signature;
} __attribute__((packed));

struct fat32_extended_boot_record {
    u32 sector_per_fat;
    u16 flags;
    u16 version;
    u32 cluster;
    u16 fsInfo;
    u16 backup_boot_sector;
    u8  reserved_0[12];

    u8  drive_number;
    u8  reserved;
    u8  signature;
    u32 volume_id;
    u8  volume_label[11];
    u8  system_identifier[8];
    u8  bootcode[420];
    u16 part_signature;
} __attribute__((packed));

struct fat16_header_compact {
    struct fat_header header;
    struct fat_extended_boot_record boot_record;
} __attribute__((packed));

struct fat32_header_compact {
    struct fat_header header;
    struct fat32_extended_boot_record boot_record;
} __attribute__((packed));

struct fat_directory_t {
    u8 filename[11];
    u8 attributes;
    u8 reserved;
    u8 creation_of_ms;
    u16 time_created;
    u16 date_created;
    u16 last_accessed_date;
    u16 high_cluster;
    u16 last_modification_time;
    u16 last_modification_date;
    u16 low_cluster;
    u32 filesize;
} __attribute__((packed));


struct fat_long_file_name_t {
    u8 order_entry;
    u8 first_char[5];
    u8 attributes;
    u8 long_type;
    u8 checksum;
    u16 second_char[6];
    u16 zeros;
    u16 last_char[2];
} __attribute__((packed));

union fat_compact {
    struct fat16_header_compact fat_16;
    struct fat32_header_compact fat_32;
} __attribute__((packed));

/**
 * OpenFATVolume()
 */
void                  OpenFATVolume(partition_table_t* partt);

/**
 * GetFATFile()
 */
struct fat_directory_t* GetFATFile(const char* filename);
void                    ReadFATFile(struct fat_directory_t* dir, char* buffer, size_t size );


#endif