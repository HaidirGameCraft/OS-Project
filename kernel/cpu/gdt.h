#pragma once
#include <stdint.h>

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


/**
 * gdt_initialize()
 * - initialize GDT with replace the GDT position to `LGDT [eax]`
 * - initialize TSS (Task State Segment) inside GDT
 */
void gdt_initialize();

/**
 * get_setentry()
 * - set the GDT entry
 */
void gdt_setentry(  int index,
                    u32 limit, 
                    u32 base,
                    u8 access_byte, 
                    u8 flags);

/**
 * gdt_install()
 * - installing GDT with GDT Descriptor
 */
extern void gdt_install(gdt_descriptor_t* gdt_desc);
