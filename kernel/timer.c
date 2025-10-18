#include <stdint.h>
#include <timer.h>
#include <cpu/idt.h>
#include <cpu/isr.h>
#include <stdio.h>
#include <cpu/port.h>

u32 t = 0;
void timer_handle( register_t reg ) {
    t++;
}

void timer_interrupt_setup() {
    set_interrupt_function((u32) &timer_handle, 32);

    //u32 count = 1000;
    //outb(0x40, count & 0xFF);
    //outb(0x40, (count & 0xFF) >> 8 );
}
