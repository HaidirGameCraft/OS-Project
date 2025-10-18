#include "../include/page.h"
#include "../include/boot_header.h"
#include "../include/bootlib.h"
#include "../include/video.h"

#define TMP_PAGE_ADDRESS    0xFF800000

u32 page_directory[ PAGE_ENTRIES ] __attribute__((aligned( MAX_PAGES )) );
u32 first_page_table[ PAGE_ENTRIES ] __attribute__((aligned( MAX_PAGES )));
u32 frame_page_table [ PAGE_ENTRIES ] __attribute__((aligned(MAX_PAGES)));

u8 page_bitmap[ MAX_PAGES ] __attribute__((aligned( MAX_PAGES )));
u8 frame_page_bitmap[ 128 ] = { 0 };

u32     Page_GetAvailablePhysicalAddress();
void    Page_SetPageBitmap( u32 address );
u8      Page_IsPhysicalAddressAvailable( u32 address );

extern void __boot_start__();
extern void __boot_end__();
extern void page_reload_cr3( u32 page_directory );
extern void invlpage( u32 address );
extern void page_enable( u32 page_directory );

void    PageInitialize() {

    zeromem( first_page_table, 0x1000 );
    zeromem( frame_page_table, 0x1000 );

    page_directory[0] = ( (u32) first_page_table) | PAGE_ATTR_PRESENT | PAGE_ATTR_READWRITE;
    printf("The Maximum Bitmap Set %x\n", MAX_PAGES * 8 * MAX_PAGES);
    // Initialize
    u32 size_page = (int)(( (u32) __boot_end__ - (u32) __boot_start__ ) / MAX_PAGES ) + 1;
    u32 physical_address = (u32) __boot_start__;
    u32 virtual_address = (u32) __boot_start__;
    for(int i = 0; i < size_page; i++)
    {
        u32 virt__ = virtual_address + i * MAX_PAGES;
        u32 phys__ = physical_address + i * MAX_PAGES;

        u32 pdIndex = PD_INDEX( virt__ );
        u32 ptIndex = PT_INDEX( virt__ );

        if ( !( page_directory[pdIndex] & PAGE_ATTR_PRESENT ) )
        {
            printf("New Page Table Required at %s", __FILE__ );
            HALT
            return;
        }
        u32* page_table = (u32*)( page_directory[pdIndex] & ~0x3FF );

        page_table[ ptIndex ] = phys__ | PAGE_ATTR_PRESENT | PAGE_ATTR_READWRITE;
        Page_SetPageBitmap( phys__ );
    }
    
    {
        u32 frame_address = TMP_PAGE_ADDRESS;
        page_directory[ PD_INDEX( frame_address ) ] = ((u32) frame_page_table) | PAGE_ATTR_PRESENT | PAGE_ATTR_READWRITE;
    }
    
    page_enable( (u32) page_directory );
    // Initialize Video framebuffer address
    u32* video_page_table = VideoGetPageTable();
    u32 framebuffer = VideoGetFramebuffer();
    u32 video_width = VideoGetWidth();
    u32 video_height = VideoGetHeight();
    u8 video_bytes_per_pixel = VideoBytesPerPixel();

    int sector_video_length = (u32)( video_width * video_height * video_bytes_per_pixel / MAX_PAGES ) + 1;
    for(int i = 0; i < sector_video_length; i++) {
        u32 addr = framebuffer + i * MAX_PAGES;
        video_page_table[ PT_INDEX( addr ) ] = addr | PAGE_ATTR_PRESENT | PAGE_ATTR_READWRITE;
    }
    page_directory[ PD_INDEX( framebuffer ) ] = ((u32) video_page_table) | PAGE_ATTR_PRESENT | PAGE_ATTR_READWRITE;
    printf("Length Of Video Memory Per Pages: %x start at PT_INDEX %x\n", sector_video_length, PT_INDEX( framebuffer ));

}

u32     PageAlloc( ) {
    u32 address = Page_GetAvailablePhysicalAddress();
    if( address == 0 )
        return 0;
    
    // Page Mapped
    frame_page_table[0] = address | PAGE_ATTR_PRESENT | PAGE_ATTR_READWRITE;
    
    zeromem( (void*) TMP_PAGE_ADDRESS, 0x1000 );
    invlpage( TMP_PAGE_ADDRESS );

    frame_page_table[0] = 0x00000000;
    page_reload_cr3( (u32) page_directory );
    return address;
}

void    PageMapping( u32 virtual_address, u8 page_flags, size_t size ) {
    u32 length = (u32)( size / MAX_PAGES ) + 1;
    for(u32 i = 0; i < length; i++) {
        u32 virt = virtual_address + i * MAX_PAGES;

        u32 pdIndex = PD_INDEX( virt );
        u32 ptIndex = PT_INDEX( virt );

        if( !( page_directory[pdIndex] & PAGE_ATTR_PRESENT ) )
        {
            u32 new_page_alloc = PageAlloc();
            if( new_page_alloc == 0 )
            {
                printf("Finding Available Physical Address is Out Of Bitmap: %s", __FILE__);
                HALT;
            }

            page_directory[pdIndex] = (new_page_alloc & ~0xFFF) | page_flags | 3;
            page_reload_cr3( (u32) page_directory );
        }
        u32* page_table = (u32*)( page_directory[pdIndex] & ~0xFFF );
        
        u32 new_physical_address = Page_GetAvailablePhysicalAddress();
        //printf("New Physical Address: %x\n", new_physical_address);
        if( new_physical_address == 0 )
        {
            printf("HALT Page, Cannot find the Physical Address\n");
            HALT;
        }
        page_table[ ptIndex ] = (new_physical_address & ~0xFFF ) | page_flags | 3;
    }
    invlpage( virtual_address );
    page_reload_cr3( (u32) page_directory );
}

u32    PageMappingPhysical( u32 virtual_address, u32 physical_address, u8 page_flags, size_t size ) {
    u32 length = (u32)( size / MAX_PAGES ) + 1;
    for(u32 i = 0; i < length; i++) {
        u32 virt = virtual_address + i * MAX_PAGES;
        u32 physc = physical_address + i * MAX_PAGES;

        //printf("%x -------> %x\n", virt, physc);
        //continue;
        
        if( Page_IsPhysicalAddressAvailable( physc ) != 0 )
        {
            printf("The Available of Physical Address required %x is already mapped, Cannot be mapped\n", physc);
            return 1;
        }
        Page_SetPageBitmap( physc );

        u32 pdIndex = PD_INDEX( virt );
        u32 ptIndex = PT_INDEX( virt ); 

        if( !( page_directory[pdIndex] & PAGE_ATTR_PRESENT ) )
        {
            u32 new_page_alloc = PageAlloc();
            printf("[DEBUG PageMappingPhysical]: Make new Page Alloc at %x\n", new_page_alloc );
            if( new_page_alloc == 0 )
            {
                printf("Finding Available Physical Address is Out Of Bitmap: %s", __FILE__);
                HALT;
            }

            page_directory[pdIndex] = ((u32) new_page_alloc & ~0xFFF ) | PAGE_ATTR_PRESENT | PAGE_ATTR_READWRITE;
            page_reload_cr3( (u32) page_directory );
        }
        u32* page_table = (u32*)( page_directory[pdIndex] & ~0xFFF );
        printf("New Physical Address: %x and Mapped on Page Table (%x): %x\n", physc, page_table, ptIndex);
        //printf("Page Index: (%x, %x)\n", pdIndex, ptIndex);
        if( physc == 0 )
        {
            printf("HALT Page, Cannot find the Physical Address\n");
            HALT;
        }
        page_table[ ptIndex ] = ((u32) physc) | PAGE_ATTR_PRESENT | PAGE_ATTR_READWRITE;
        printf("[DEBUG] Page_Table[ %x ]: %x\n", ptIndex, page_table[ ptIndex ]);
    }

    //__asm__ volatile("invlpg (%0)" :: "r"( virtual_address ) : "memory" );
    //page_reload_cr3( (u32) page_directory );
    return 0;
}

void    PageTableUsed() {
    for(int i = 0; i < PAGE_ENTRIES; i++) {
        if( page_directory[i] != 0 )
            printf("Page Directory[%x]: %x\n", i, page_directory[i]);
    }
}

void    PageTableAttach(u32 address) {
    u32 pd_index = PD_INDEX( address );

    if( page_directory[pd_index] == 0 )
        return;

    printf("Page Directory[%x]: %x\n", pd_index, page_directory[pd_index]);
    u32 ptaddr = page_directory[pd_index] & ~0xFFF;
    u32* page_table = (u32*) ptaddr;

    for(int i = 0; i < PAGE_ENTRIES; i++) {
        if( page_table[i] != 0 )
            printf("Page Directory[%x] -> Page Table[%x]: %x\n", pd_index, i, page_table[i]);
    }
}

u32     Page_GetAvailablePhysicalAddress() {
    u32 physical_address = 0;
    for(int i = 1; i < MAX_PAGES * 8; i++)
    {
        physical_address = i * MAX_PAGES;
        if( Page_IsPhysicalAddressAvailable( physical_address ) != 0 )
            continue;

        Page_SetPageBitmap( physical_address );
        break;
    }
    return physical_address;
}

void    Page_SetPageBitmap( u32 address ) {
    u32 page = address >> 12;
    if( page >= MAX_PAGES * 8 * 0x1000 )
    {
        printf("Page SetBitmap HALT: %x Out of %x\n", address, MAX_PAGES * 8 * MAX_PAGES);
        HALT;
    }

    u32 bitmap_index = (u32)( page / 8 );
    u32 bitmap_offset = page % 8;
    page_bitmap[ bitmap_index ] |= 1 << bitmap_offset;
}

u8      Page_IsPhysicalAddressAvailable( u32 address ) {
    u32 page = address >> 12;
    u32 bitmap_index = (u32)( page / 8 );
    u32 bitmap_offset = page % 8;
    return (page_bitmap[ bitmap_index ] & (1 << bitmap_offset ));
}

void    PageCheckAddress(u32 address) {
    for(int i = 0; i < PAGE_ENTRIES; i++) {
        if( page_directory[i] == 0 )
            continue;

        u32* page_table = (u32*)( page_directory[i] & ~0xFFF );
        int j = 0;
        while( j < PAGE_ENTRIES) {
            u32 addr = page_table[j] & ~0xFFF;
            if( addr == address )
            {
                printf("Paging[ %x : %x ] = %x\n", i, j, page_table[j]);
            }
            j++;
        }
    }
}