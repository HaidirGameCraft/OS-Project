#pragma once
#include <stdint.h>

struct partition_table_t {
    uint8_t  attributes;
    uint8_t  chs_start[3];
    uint8_t  part_type;
    uint8_t  chs_end[3];
    uint32_t lba_start;
    uint32_t sector_partition;
} __attribute__((packed));

struct mbr_struct_t {
    uint8_t  bootcode[440];
    uint32_t signature;
    uint16_t optional;
    struct partition_table_t partition[4];
    uint16_t last_signature;
} __attribute__((packed));


// File Allocator Table structure

struct fat_header {
    uint8_t  jmp_code[3];
    uint8_t  oem_identifier[8];
    uint16_t bytes_per_sector;
    uint8_t  sector_per_cluster;
    uint16_t reserved_sector;
    uint8_t  fat_number;
    uint16_t root_dir_entries;
    uint16_t total_sector;
    uint8_t  media_type;
    uint16_t sector_per_fat;
    uint16_t sector_per_track;
    uint16_t heads;
    uint32_t hidden_sectors;
    uint32_t large_sectors;
} __attribute__((packed));

struct fat_extended_boot_record {
    uint8_t  drive_number;
    uint8_t  reserved;
    uint8_t  signature;
    uint32_t volume_id;
    uint8_t  volume_label[11];
    uint8_t  system_identifier[8];
    uint8_t  bootcode[448];
    uint16_t part_signature;
} __attribute__((packed));

struct fat32_extended_boot_record {
    uint32_t sector_per_fat;
    uint16_t flags;
    uint16_t version;
    uint32_t cluster;
    uint16_t fsInfo;
    uint16_t backup_boot_sector;
    uint8_t  reserved_0[12];

    uint8_t  drive_number;
    uint8_t  reserved;
    uint8_t  signature;
    uint32_t volume_id;
    uint8_t  volume_label[11];
    uint8_t  system_identifier[8];
    uint8_t  bootcode[448];
    uint16_t part_signature;
} __attribute__((packed));

struct fat_directory_t {
    uint8_t filename[11];
    uint8_t attributes;
    uint8_t reserved;
    uint8_t creation_of_ms;
    uint16_t time_created;
    uint16_t date_created;
    uint16_t last_accessed_date;
    uint16_t high_cluster;
    uint16_t last_modification_time;
    uint16_t last_modification_date;
    uint16_t low_cluster;
    uint32_t filesize;
} __attribute__((packed));

struct long_file_name_t {
    uint8_t order_entry;
    uint8_t first_char[5];
    uint8_t attributes;
    uint8_t long_type;
    uint8_t checksum;
    uint16_t second_char[6];
    uint16_t zeros;
    uint16_t last_char[2];
} __attribute__((packed));