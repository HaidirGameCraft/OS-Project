#pragma once

#include "stdint.h"

// TSS Header
typedef struct {
    u32 link;
    u32 esp0;
    u32 ss0;
    u32 esp1;
    u32 ss1;
    u32 esp2;
    u32 ss2;
    u32 cr3;
    u32 eip;
    u32 eflags;
    u32 eax, ecx, edx, ebx, esp, ebp, esi, edi;
    u32 es, cs, ss, ds, fs, gs, ldtr;
    u32 iobp;
    u32 ssp;
} __attribute__((packed)) tss_t;

// GDT Header
typedef struct {
    u16 low_limit;
    u16 low_base;
    u8 mid_base;
    u8 access_byte;
    u8 flags;
    u8 high_base;
} __attribute__((packed)) gdt_entry_t;

typedef struct {
    u16 limit;
    u32 base;
} __attribute__((packed)) gdt_descriptor_t;

void gdt_initialize();
void gdt_setentry(  int index,
                    u32 limit, 
                    u32 base,
                    u8 access_byte, 
                    u8 flags);
extern void gdt_install(gdt_descriptor_t* gdt_desc);

// ISR Header
typedef struct {
    u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
    u32 number_interrupt;
    u32 error_code;
} __attribute__((packed)) register_t;

void isr_handle(register_t reg);

// IDT Header
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

void idt_initialize();
void idt_setentry(  int number,
                    u32 offset,
                    u16 segment, 
                    u8 attribute);
void set_interrupt_function(u32 address, int index);
extern void idt_install(idt_desc* idt_ptr);


// Port function
// iportb -> Input Port Byte
extern u8   iportb( u16 port );
// iportw -> Input Port Word
extern u16  iportw( u16 port );

// oportb -> Output Port Byte
extern void oportb( u16 port, u8 data );
// oportw -> Output Port Word
extern void oportw( u16 port, u16 data );