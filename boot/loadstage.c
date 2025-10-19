
#include "boot_stage/include/stdint.h"
#include "boot_stage/include/boot_header.h"

struct boot_table_t {
    u16 partition_offset;
    u32 lba_partition;
} __attribute__((packed));

struct fat_header {
    u8  jmp_code[3];
    u8  oem[8];
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
} __attribute__((packed)) fat_header;

struct fat32_extended_boot_record {
    u32 sectors_per_fat;
    u16 flags;
    u16 fat_version;
    u32 cluster_number;
    u16 sector_of_fsinfo;
    u16 sector_per_backup;
    u8  reserved[12];
} __attribute__((packed));

struct fat_boot_record {
    u8  drive_number;
    u8  flags_wnt;
    u8  signature;
    u32 volume_id;
    u8  volume_label[11];
    u8  system[8];
} __attribute__((packed));

struct fat_directory {
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
} __attribute__((packed));

extern void boot_table();
struct fat_header* fheader;
struct fat32_extended_boot_record* f32br;
struct fat_boot_record* fbr;
struct video_mode_header* video_mode_ptr;
struct boot_table_t* bt_;
u8 buffer[512] = {0};
u8 typeFat = 0;

extern u8 inb(u16 port);
extern u16 inw(u16 port);
extern void outb(u16 port, u8 data);
extern void outw(u16 port, u16 data);

u8 strncmp(char* _1, char* _2, u16 size);
void read_disk(u32 lba, u8* buffer );
void read_fat_data(struct fat_directory* dir, u32 address);

void load_stage() {

    extern void vesa_mode();
    bt_ = (struct boot_table_t*) boot_table;
    video_mode_ptr = (struct video_mode_header*) vesa_mode;

    fheader = (struct fat_header*) 0x7c00;
    if( fheader->large_sector < 2048 )
    {
        typeFat = 12;
        fbr = (struct fat_boot_record*)( 0x7c00 + sizeof( struct fat_header ) );
    }
    else if ( fheader->large_sector < 65536 )
    {
        typeFat = 16;
        fbr = (struct fat_boot_record*)( 0x7c00 + sizeof( struct fat_header ) );
    }
    else {
        typeFat = 32;
        f32br = (struct fat32_extended_boot_record*)( 0x7c00 + sizeof( struct fat_header ) );
        fbr = (struct fat_boot_record*)( 0x7c00 + sizeof( struct fat_header ) + sizeof( struct fat32_extended_boot_record ) );
    }


    // Fat 12/16

    u32 fat_start = bt_->lba_partition + fheader->reserved_sectors;
    u32 root_dir_start = fat_start + fheader->sectors_per_fat * fheader->fat_number;
    u32 root_dir_size = fheader->root_dir_entries;

    ((u16*) 0xb8000)[0] = 0x0F00 | ( (root_dir_start & 0x0F) + '0');
    ((u16*) 0xb8000)[1] = 0x0F00 | (((root_dir_start >> 4) & 0x0F) + '0');
    ((u16*) 0xb8000)[2] = 0x0F00 | (((root_dir_start >> 8) & 0x0F) + '0');
    ((u16*) 0xb8000)[3] = 0x0F00 | (((root_dir_start >> 12) & 0x0F) + '0');


    read_disk( root_dir_start, buffer );
    
    for(int i = 0; i < root_dir_size; i++) {
        struct fat_directory* dir = &((struct fat_directory*) buffer)[i];
        
        if( strncmp( dir->filename, "BOOTX32 BIN ", 11) )
        {
            ((u16*) 0xb8000)[4] = 0x0E00 | ( 'K' );
            read_fat_data( dir, 0x10000 );
            break;
        }   
    }

    // Fat32 Code
    // u32 cluster = fbr->cluster_number;
    // u32 fat_start = fheader->reserved_sectors;
    // u32 data_start = fat_start + fbr->sectors_per_fat * fheader->fat_number;
    
    // while ( cluster < 0x0FFFFFF0 )
    // {
    //     ((u16*) 0xb8000)[0] = 0x0F00 | ((cluster & 0x0F) + '0');
    //     ((u16*) 0xb8000)[1] = 0x0F00 | (((cluster >> 4) & 0x0F) + '0');
    //     ((u16*) 0xb8000)[2] = 0x0F00 | (((cluster >> 8) & 0x0F) + '0');
    //     ((u16*) 0xb8000)[3] = 0x0F00 | (((cluster >> 12) & 0x0F) + '0');
    //     u32 cluster_segment = data_start + fheader->sectors_per_cluster * (cluster - 2);
    //     read_disk(cluster_segment, buffer);
        
    //     for(int i = 0; i < 16; i++) {
    //         struct fat_directory* dir = &((struct fat_directory*) buffer)[i];
    //         if( strncmp( dir->filename, "BOOTX32 BIN ", 11) )
    //         {
    //             ((u16*) 0xb8000)[4] = 0x0E00 | 'K';
    //             read_fat_data( dir, 0x10000 );
    //             break;
    //         }
    //     }

    //     u32 fat_sector = (u32)( cluster / 128 );
    //     u32 fat_index = cluster % 128;
    //     read_disk(fat_start + fat_sector, buffer);
    //     cluster = ((u32*) buffer)[fat_index];
    // }

    struct boot_header* bootheader = (struct boot_header*) 0x10000;
    if( bootheader->magic_number != 0x3B002D1F )
    {
        // ((u16*) 0xb8000)[0] = 0x0F00 | '1';
        // ((u16*) 0xb8000)[1] = 0x0F00 | '0';
        // ((u16*) 0xb8000)[2] = 0x0F00 | 'A';
        // ((u16*) 0xb8000)[3] = 0x0F00 | 'B';
        return;
    }

    // ((u16*) 0xb8000)[0] = 0x0F00 | ( (bootheader->entry_start & 0x0F) + '0');
    // ((u16*) 0xb8000)[1] = 0x0F00 | (((bootheader->entry_start >> 4) & 0x0F) + '0');
    // ((u16*) 0xb8000)[2] = 0x0F00 | (((bootheader->entry_start >> 8) & 0x0F) + '0');
    // ((u16*) 0xb8000)[3] = 0x0F00 | (((bootheader->entry_start >> 12) & 0x0F) + '0');

    bootheader->video_mode_ptr = (u32) video_mode_ptr;
    void (*go_to_boot)(struct boot_header*) = (void (*)(struct boot_header*)) bootheader->entry_start;
    go_to_boot( bootheader );

    return;
}

void read_fat_data( struct fat_directory* dir, u32 address ) {
    u32 cluster = dir->high_cluster << 16 | dir->low_cluster; 
    u32 fat_start = bt_->lba_partition + fheader->reserved_sectors;
    u32 data_disk = 0;

    if( typeFat == 32 )
        data_disk = fat_start + f32br->sectors_per_fat * fheader->fat_number;
    else
        data_disk = fat_start + fheader->sectors_per_fat * fheader->fat_number + fheader->root_dir_entries * sizeof( struct fat_directory ) / fheader->bytes_per_sectors ;

    // ((u16*) 0xb8000)[0] = 0x0F00 | ( (typeFat & 0x0F) + '0');
    // ((u16*) 0xb8000)[1] = 0x0F00 | (((typeFat >> 4) & 0x0F) + '0');
    // ((u16*) 0xb8000)[2] = 0x0F00 | (((typeFat >> 8) & 0x0F) + '0');
    // ((u16*) 0xb8000)[3] = 0x0F00 | (((typeFat >> 12) & 0x0F) + '0');

    u32 end_of_limit = 0xFFF0;
    if( typeFat == 32 ) end_of_limit = 0x0FFFFFF0;

    while( cluster < end_of_limit ) {
        u32 cluster_segment = data_disk + fheader->sectors_per_cluster * (cluster - 2);

        ((u16*) 0xb8000)[0] = 0x0F00 | ( (cluster_segment & 0x0F) + '0');
        ((u16*) 0xb8000)[1] = 0x0F00 | (((cluster_segment >> 4) & 0x0F) + '0');
        ((u16*) 0xb8000)[2] = 0x0F00 | (((cluster_segment >> 8) & 0x0F) + '0');
        ((u16*) 0xb8000)[3] = 0x0F00 | (((cluster_segment >> 12) & 0x0F) + '0');

        for(int i = 0; i < fheader->sectors_per_cluster; i++) {
            read_disk(cluster_segment + i, (u8*) address );
            address += 0x200;
        }

        if( typeFat == 32 )
        {
            u16 byte_per_cluster = fheader->bytes_per_sectors / sizeof( u32 );
            u32 fat_sector = (int)( cluster / byte_per_cluster );
            u32 fat_index = cluster % byte_per_cluster;
            read_disk(fat_start + fat_sector, buffer);
            cluster = ((u32*) buffer)[fat_index];
        }
        else {
            u16 byte_per_cluster = fheader->bytes_per_sectors / sizeof( u16 );
            u32 fat_sector = (int)( cluster / byte_per_cluster );
            u32 fat_index = cluster % byte_per_cluster;
            read_disk(fat_start + fat_sector, buffer);
            cluster = ((u16*) buffer)[fat_index];
        }
    }
}

u8 strncmp(char* _1, char* _2, u16 size) {
    for(int i = 0; i < size; i++) {
        if( _1[i] != _2[i] )
            return 0;
    }

    return 1;
}

void read_disk(u32 lba, u8* buffer ) {
    outb(0x1F6, 0xE0 | (lba >> 24 & 0x0F) ); // settting up "master" or "slave" by 0xE0 or 0xF0
    //outb(0x1F1, 0x00);
    outb(0x1F2, 1);
    outb(0x1F3, (u8) lba & 0xFF);
    outb(0x1F4, (u8) (lba >> 8) & 0xFF);
    outb(0x1F5, (u8) (lba >> 16) & 0xFF);

    outb(0x1F7, 0x20); // send read command

    while( (inb(0x1F7) & 0x80) );

    for(int i = 0; i < 256; i++) {
        ((u16*) buffer)[i] = inw( 0x1F0 );
    }
}
