#pragma once

#include <stdint.h>

#define PAGE_ENTRIES 1024
#define PAGE_SIZE    4096

#define PD_INDEX( x ) ( (x >> 22) & 0x3FF )
#define PT_INDEX( x ) ( (x >> 12) & 0x3FF )

#define PAGE_ATTR_PRESENT       1 << 0
#define PAGE_ATTR_READWRITE     1 << 1

typedef u32 page_entry_t;

struct page_table_t {
    page_entry_t entries[ PAGE_ENTRIES ];
} __attribute__((packed));

struct page_directory_t {
    page_entry_t entries[ PAGE_ENTRIES ];
} __attribute__((packed));

/**
 * page_initalize();
 * - initalize the first table for mapped kernel at specific address
 * - initalize the video memory table that mapped on higher address
 * - initalize the last table on pd[LAST] -> pt[LAST] for temporary paging
 * - initalize the first table pt[0] to make it as page bitmap
 */
void page_initialize();

/**
 * page_map()
 * - mapped the address to available physical address
 */
void page_map(u32 address, u8 page_attributes, size_t size );

/**
 * page_pmap() (NOT IMPLEMENT)
 * - mapped the physical address when it available
 */
void page_pmap(u32 physicl_address, u8 page_attributes);

/**
 * page_vmap() (NOT IMPLEMENT)
 * - mapped the virtual address to physical address
 */
void page_vmap(u32 virtual_address, u32 physical_address, u8 page_attributes, size_t size );

/**
 * page_enable()
 * - set the address of page directory to 'CR3' register
 * - enable paging 
 */
extern void page_enable( u32 addr );

/**
 * page_reload()
 * - reload or set page directory
 */
extern void page_reload( u32 addr );