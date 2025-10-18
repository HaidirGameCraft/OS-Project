#pragma once

#include <stdint.h>
// https://wiki.osdev.org/Interrupts_Tutorial -> Interrupt Tutorial from OSDev
typedef struct {
    u16 low_offset;
    u16 segment;
    u8 reserved;
    u8 attributes;
    u16 high_offset;
} __attribute__((packed)) idt_entry_t;

typedef struct {
    u16 limit;
    u32 base;
} __attribute__((packed)) idt_desc;

/**
 * idt_initalize()
 * - Initialize all ISR and IRQ function
 */
void idt_initialize();

/**
 * idt_setentry()
 * - set IDT entry
 */
void idt_setentry(  int number,
                    u32 offset,
                    u16 segment, 
                    u8 attribute);

/**
 * set_interrupt_function()
 * - set the interrupt function on specific IRQ
 * @param address - must be in [ void (*)(register_t) ]
 */
void set_interrupt_function(u32 address, int index);
extern void idt_install(idt_desc* idt_ptr);
