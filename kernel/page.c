#include <page.h>
#include <string.h>
#include <stdio.h>

#include "../boot/boot_stage/include/boot_header.h"

u32 tmp_page_table_address = 0;

struct page_table_t* kernel_table __attribute__((aligned(PAGE_SIZE)));
struct page_table_t* first_table __attribute__((aligned(PAGE_SIZE)));
struct page_table_t* last_table __attribute__((aligned(PAGE_SIZE)));
struct page_directory_t* page_directory __attribute__((aligned(PAGE_SIZE)));
u8* page_bitmap = 0x00000000;

/**
 * page_alloc()
 * - finding the available address to make new page table
 * @return the address of page table
 */
u32 page_alloc();

/**
 * page_address_is_available()
 * - checking the address that is available on page bitmap
 * @return 0 = availabe, 1 = not available
 */
u32  page_address_is_available( u32 addr );
void page_set_address( u32 addr );
void page_set_free_address( u32 addr );

/**
 * page_find_available_address()
 * - finding the available address on page map
 * @return returning the physical address
 */
u32  page_find_available_address();

/**
 * page_tmp_map()
 * - mapped the address to the last table in page directory[1023]
 */
void page_tmp_map(u32 addr);

/**
 * page_tmp_unmap()
 * - unmapped the address on the last tbale in page_direcotry[1023]
 */
void page_tmp_unmap();

void page_initialize() {
    extern void __kernel_start();
    extern void __kernel_end();
    
    u32 krlstart = (u32) __kernel_start;
    u32 krlend = (u32) __kernel_end;

    page_directory = (struct page_directory_t*) 0x1000;
    kernel_table = (struct page_table_t*) 0x2000;
    first_table = (struct page_table_t*) 0x3000;
    last_table = (struct page_table_t*) 0x4000;


    
    zeromem( page_directory->entries, 0x1000 );
    zeromem( kernel_table->entries, 0x1000 );
    zeromem( first_table->entries, 0x1000 );
    
    
    u32 size_krl = krlend - krlstart;
    u32 size_page = ((int)( size_krl / PAGE_SIZE ) ) + 1;
    for(int i = 0; i < size_page; i++) {
        u32 vaddr = ( krlstart & ~0xFFF ) + i * PAGE_SIZE;
        u32 addr = (krlstart - 0xC0000000) + i * PAGE_SIZE;

        //printf("Mapped: Virt %x -> %x\n", vaddr, addr);
        
        //first_table->entries[ PT_INDEX( addr ) ] = addr | 3;
        kernel_table->entries[ PT_INDEX( vaddr ) ] = addr | PAGE_ATTR_PRESENT | PAGE_ATTR_READWRITE;
    }
    
    page_directory->entries[ PD_INDEX( krlstart ) ] = ((u32) kernel_table->entries ) | PAGE_ATTR_PRESENT | PAGE_ATTR_READWRITE;
    page_directory->entries[ 0 ] = ((u32) first_table->entries) | PAGE_ATTR_PRESENT | PAGE_ATTR_READWRITE;
    
    for(int i = 1; i <= 5; i++) {
        first_table->entries[i] = i * PAGE_SIZE | PAGE_ATTR_PRESENT | PAGE_ATTR_READWRITE;
    }

    page_reload( (u32) page_directory->entries );
    

    // initialize page map for video memory table
    extern struct video_mode_header video_mode;
    struct page_table_t* video_memory_table = (struct page_table_t*) 0x5000;
    
    u32 size_video = video_mode.width * video_mode.height * video_mode.bits_per_pixel / 8;
    size_page = ((int)( size_video / PAGE_SIZE )) + 1;
    
    for(int i = 0; i < size_page; i++) 
    {
        u32 addr = ( video_mode.framebuffer & ~0xFFF ) + i * PAGE_SIZE;
        
        video_memory_table->entries[ PT_INDEX( addr ) ] = addr | PAGE_ATTR_PRESENT | PAGE_ATTR_READWRITE;
    }
    page_directory->entries[ PD_INDEX( video_mode.framebuffer) ] = ((u32) video_memory_table->entries ) | PAGE_ATTR_PRESENT | PAGE_ATTR_READWRITE;
    

    // set the last address for tmp page table
    tmp_page_table_address = ( 1023 << 22 ) | ( 1023 << 12 );
    first_table->entries[0] = ((u32) page_bitmap) | PAGE_ATTR_PRESENT | PAGE_ATTR_READWRITE;
    // set 1 bits on first bitmap page
    page_bitmap[0] |= 0b11111;
}


void page_map(u32 address, u8 page_attributes, size_t size ) {
    int size_page = ((int)( size / PAGE_SIZE)) + 1;

    for(u32 i = 0; i < size_page; i++) {
        u32 addr = ( address & ~0xFFF ) + i * PAGE_SIZE;

        u32 pd = PD_INDEX( addr );
        u32 pt = PT_INDEX( addr );
        if( !(page_directory->entries[pd] & PAGE_ATTR_PRESENT) )
        {
            u32 addr_page_table = page_alloc();
            // when the addr page table cannot find the available address, it return -1 as the error state
            if( addr_page_table == -1 )
            {
                printf("[ERROR]: Cannot making new page table\n");
                return;
            }


            page_directory->entries[pd] = addr_page_table | PAGE_ATTR_PRESENT | PAGE_ATTR_READWRITE;
        }

        struct page_table_t* page_table = (struct page_table_t*)( page_directory->entries[pd] & ~0xFFF );
        u32 physical_addr = page_find_available_address();
        
        page_table->entries[pt] = physical_addr | page_attributes;
    }
    __asm__ volatile("invlpg (%0)" :: "r"(address) : "memory");
}

void page_pmap(u32 physical_address, u8 page_attributes) {

}

void page_vmap(u32 virtual_address, u32 physical_address, u8 page_attributes, size_t size ) {
    int size_page = ((int)( size / PAGE_SIZE)) + 1;

    for(u32 i = 0; i < size_page; i++) {
        u32 vaddr = ( virtual_address & ~0xFFF ) + i * PAGE_SIZE;
        u32 paddr = ( physical_address  & ~0xFFF ) + i * PAGE_SIZE;

        u32 pd = PD_INDEX( vaddr );
        u32 pt = PT_INDEX( vaddr );
        if( !(page_directory->entries[pd] & PAGE_ATTR_PRESENT) )
        {
            u32 addr_page_table = page_alloc();
            // when the addr page table cannot find the available address, it return -1 as the error state
            if( addr_page_table == -1 )
            {
                printf("[ERROR]: Cannot making new page table\n");
                return;
            }


            page_directory->entries[pd] = addr_page_table | PAGE_ATTR_PRESENT | PAGE_ATTR_READWRITE;
        }

        struct page_table_t* page_table = (struct page_table_t*)( page_directory->entries[pd] & ~0xFFF );
        if( page_address_is_available( paddr ) )
            paddr = page_find_available_address();
        else
            page_set_address( paddr );
        
        page_table->entries[pt] = paddr | page_attributes;
    }
    __asm__ volatile("invlpg (%0)" :: "r"(virtual_address) : "memory");
}


void page_unmap(u32 virtual_address, size_t size )
{
    struct page_directory_t* cpdir = (struct page_directory_t*) page_getdir();
    if( cpdir == NULL )
    {
        printf("[PAGING ERROR] cpdir is not served\n");
        return;
    }

    int size_page = ((int)( size / PAGE_SIZE)) + 1;

    for(u32 i = 0; i < size_page; i++) {
        u32 addr = ( virtual_address & ~0xFFF ) + i * PAGE_SIZE;

        u32 pd = PD_INDEX( addr );
        u32 pt = PT_INDEX( addr );
        if( !(page_directory->entries[pd] & PAGE_ATTR_PRESENT) )
            continue;   // continue the forloop when the page_directory->entries[pd] is not have the page_table

        struct page_table_t* page_table = (struct page_table_t*)( page_directory->entries[pd] & ~0xFFF );
        u32 physical_address = page_table->entries[pt] & ~0xFFF;
        if( physical_address != 0 )
            page_set_free_address( physical_address );
        
        page_table->entries[pt] = 0x00000000;
    }
    __asm__ volatile("invlpg (%0)" :: "r"(virtual_address) : "memory");

}


u32 page_alloc() {
    u32 addr = page_find_available_address();
    if( addr == 0 )
        return -1;         // returning -1 becausethe address is not available

    page_tmp_map( addr );   // map the address to the tmp paging for access read or write
    
    zeromem((void*) tmp_page_table_address, 0x1000 );   // clearing the random value that available on the address
    page_tmp_unmap();       // unmap the address when not use
    return addr;
}

void page_tmp_unmap() {
    u32 pd_i = PD_INDEX( tmp_page_table_address );
    u32 pt_i = PT_INDEX( tmp_page_table_address );

    if( !( page_directory->entries[pd_i] & PAGE_ATTR_PRESENT ) )
        return;     // returning because the final table is not found

    struct page_table_t* page_table = (struct page_table_t*)( page_directory->entries[pd_i] & ~0xFFF );

    page_table->entries[pt_i] = 0;
}


void page_tmp_map(u32 addr) {
    u32 pd_i = PD_INDEX( tmp_page_table_address );
    u32 pt_i = PT_INDEX( tmp_page_table_address );

    if( !( page_directory->entries[pd_i] & PAGE_ATTR_PRESENT ) )
        page_directory->entries[pd_i] = ((u32) last_table->entries) | PAGE_ATTR_PRESENT | PAGE_ATTR_READWRITE;

    struct page_table_t* page_table = (struct page_table_t*)( page_directory->entries[pd_i] & ~0xFFF );

    page_table->entries[pt_i] = ((u32) addr) | PAGE_ATTR_PRESENT | PAGE_ATTR_READWRITE;
}

u32  page_address_is_available( u32 addr ) {
    u8 p = page_bitmap[ ((int)( (addr >> 12) / 8 )) ];
    return !((p >> ( (addr >> 12) % 8 )) & 1);
}

void page_set_address( u32 addr ) {
    page_bitmap[ ((int)( (addr >> 12) / 8 )) ] |= 1 << ((addr >> 12) % 8);
}

void page_set_free_address( u32 addr ) {
    page_bitmap[ ((int)( (addr >> 12) / 8 )) ] = ~(0 << ((addr >> 12) % 8));
}

u32  page_find_available_address() {
    u8* avp = 0;
    u8 v = 0;
    u8 j = 0;
    for(int i = 0; i < PAGE_SIZE; i++) {
        if( page_bitmap[i] != 0xFF )
        {
            avp = &page_bitmap[i];
            v = 1;
            j = i;
            break;
        }
    }

    if( v == 0 )
        return 0;

    for(int i = 0; i < 8; i++) {
        if( !(*avp >> i & 1) )
        {
            u32 addr = (j * 8 + i) * PAGE_SIZE;
            *avp |= 1 << i;
            return addr;
        }
    }

    return 0;
}