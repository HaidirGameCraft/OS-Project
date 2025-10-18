#pragma once

/**
 * FileSystem
 * 
 * ATAIO Referrence: https://wiki.osdev.org/ATA_PIO_Mode
 */

#define PRIMARY_ATA         0x1F0
#define ATA_DATA_REGISTER   0x01
#define ATA_SECTOR_COUNT    0x02
#define ATA_LBA_LOW         0x03
#define ATA_LBA_MID         0x04
#define ATA_LBA_HIGH        0x05
#define ATA_DRIVE           0x06
#define ATA_STATUS_REGISTER 0x07
#define ATA_CMD_REGISTER    0X07

#define ATA_MASTER 0xE0
#define ATA_SLAVE  0xF0

#define READ_DISK_CMD       0x20
#define WRITE_DISK_CMD      0x30

#include "boot_header.h"
#include "stdint.h"

typedef struct {
    u8 jmp_code[3];
    u8 oem_identifier[8];
    u16 bytes_per_sectors;
    u8  sectors_per_cluster;
    u16 reserved_sectors;
    u8  fat_number;
    u16 root_dir_entries;
    u16 total_sectors;
    u8  media_type;
    u16 sectors_per_fat;
    u16 sectors_per_track;
    u16 heads;
    u32 hiddeh_sectors;
    u32 large_sector;
} __attribute__((packed)) FATHeader;

typedef struct {
    u32 sectors_per_fat;
    u16 flags;
    u16 fat_version;
    u32 cluster_number;
    u16 sector_of_fsinfo;
    u16 sector_per_backup;
    u8  reserved[12];
} __attribute__((packed)) FAT32ExtendedBootRecord;

typedef struct {
    u8  drive_number;
    u8  flags_wnt;
    u8  signature;
    u32 volume_id;
    u8  volume_label[11];
    u8  system[8];
} __attribute__((packed)) FATBootRecord;

typedef struct {
    FATHeader header;
    FAT32ExtendedBootRecord extended;
    FATBootRecord boot_record;
} __attribute__((packed)) FATHeaderCompact;

typedef struct {
    u8 filename[11];
    u8 attributes;
    u8 reserved;
    u8 creation_times_of_dm;
    u16 times;
    u16 date;
    u16 last_access_date;
    u16 high_cluster;
    u16 last_modification_time;
    u16 last_modification_date;
    u16 low_cluster;
    u32 filesize;
} __attribute__((packed)) FATDirectory;

/**
 * MBR Table
 * Referrence - [https://wiki.osdev.org/MBR_(x86)]
 * Contains MBR Header and Partition Table
 */

typedef struct {
    u8  attributes;
    u8  chs_address_start[3];
    u8  partition_type;
    u8  chs_address_end[3];
    u32 lba_start;
    u32 numbers_of_sectors;
} __attribute__((packed)) Partition_Table;

typedef struct {
    u8  boot_code[440];  // The first 440 code
    u32 unique_id;
    u16 reserved;
    Partition_Table partitions[4];
    u16 sign_bootsector;
} __attribute__((packed)) MBR_Header;

typedef struct {
    u8 type;
    void* ptr;
} __attribute__((packed)) BootDiskInfo;

typedef struct {
    u8 type;
    Partition_Table* partition;
    void* volume_header;
} VolumeInfo;

enum VolumeType {
    VolumeType_None,
    VolumeType_Fat12,
    VolumeType_Fat16,
    VolumeType_Fat32
};

enum TypeDisk {
    TypeDisk_Unknown,
    TypeDisk_MBR
};

void InitializeDisk();
BootDiskInfo GetDiskInfo();
VolumeInfo OpenVolume(BootDiskInfo* bdiskinfo, int partition_number);

// FAT Tools
FATDirectory FAT_FindFile(VolumeInfo* volumeinfo, const char* filename );

/**
 * FAT_ReadFile
 * @param volumeInfo
 * @param directory
 * @param buffer - must 512 bytes size
 */
void         FAT_ReadFile(VolumeInfo* volumeInfo, FATDirectory* directory, char* buffer );

void ReadDisk( u32 lba, char* buffer );
void WriteDisk( u32 lba, char* buffer );