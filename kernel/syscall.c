#include <syscall.h>
#include <cpu/isr.h>
#include <memory.h>
#include <fs.h>
#include <page.h>
#include <string.h>
#include <stdio.h>
/**
 * syscall_interrupt()
 * - @param reg - register 32 bits
 */
u32 syscall_interrupt( register_t reg ) {
    u32 value_ret = 0;
    if( reg.eax == 0x01 )               // Read
    {
        u32 size = reg.ecx;
        u32 id = reg.edx;
        u32 buffer = reg.ebx;

        if( id != NULL )
        {
            struct fat_directory_t* dir = (struct fat_directory_t*) id;
            ReadFATFile( dir, (char*) buffer, size );
        }
        else {
            printf("[SYSCALL ERROR] (read): Cannot finding the ID( FILE ) or ID( FILE ) == 0\n");
        }
    }
    else if ( reg.eax == 0x02 )         // Write
    {
        //TODO
    }
    else if ( reg.eax == 0x03 )         // Open
    {
        const char* filename = (const char*) reg.ebx;
        if( filename == 0 )
        {
            printf("[SYSCALL ERROR] (open): filename is not found or empty\n");
            return value_ret;
        }

        value_ret = (u32) GetFATFile( filename );
        if( value_ret == 0 )
        {
            printf("[SYSCALL]: %s, file not found\n", filename);
        }
    }
    else if ( reg.eax == 0x04 )         // Close
    {
        u32 id = reg.edx;
        if( id == NULL )
        {
            printf("[SYSCALL ERROR] (close): the ID( FILE ) is NULL or there is not ID( FILE )\n");
            return value_ret;
        }

        free( (void*) id );
    }
    else if ( reg.eax == 0x05 )         // Page Map
    {
        u32 virt_addr = reg.ebx;
        size_t size = reg.ecx;
        u8 flags = reg.edx;

        page_map( virt_addr, flags, size );        
    }
    else if ( reg.eax == 0x06 )         // Page VirtualMap
    {
        u32 virt_addr = reg.ebx;
        u32 phys_addr = reg.edi;
        size_t size = reg.ecx;
        u8 flags = reg.edx;
        
        page_vmap(virt_addr, phys_addr, flags, size );
    }
    else if ( reg.eax == 0x07 )         // Page Unmap
    {
        u32 virt_addr = reg.ebx;
        u32 size = reg.ecx;
        page_unmap( virt_addr, size );
    }

    return value_ret;
}