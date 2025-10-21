#include <syscall.h>
#include <cpu/isr.h>

/**
 * syscall_interrupt()
 * - @param reg - register 32 bits
 */
u32 syscall_interrupt( register_t reg ) {
    u32 value_ret = 0;
    if( reg.eax == 0x01 )               // Read
    {

    }
    else if ( reg.eax == 0x02 )         // Write
    {

    }
    else if ( reg.eax == 0x03 )         // Open
    {

    }
    else if ( reg.eax == 0x04 )         // Close
    {

    }
    else if ( reg.eax == 0x05 )         // Page Map
    {

    }

    return value_ret;
}