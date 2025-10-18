#include "../include/cpu.h"
#include "../include/stdio.h"

#define CODE_FLAGS 0x0C
#define DATA_FLAGS 0x0C

gdt_entry_t gdt_entries[6];
gdt_descriptor_t gdt_ptr;

tss_t task_desc;

void gdt_initialize() {
    // initialize gdt entries
    gdt_setentry(0, 0x00000000, 0x00000000, 0x00, 0x00); // NULL GDT ENTRY
    gdt_setentry(1, 0xFFFFFFFF, 0x00000000, 0x9A, CODE_FLAGS); // CODE GDT ENTRY
    gdt_setentry(2, 0xFFFFFFFF, 0x00000000, 0x92, DATA_FLAGS); // DATA GDT ENTRY
    gdt_setentry(3, 0xFFFFFFFF, 0x00000000, 0xFA, CODE_FLAGS); // CODE 16BITS GDT ENTRY
    gdt_setentry(4, 0xFFFFFFFF, 0x00000000, 0xF2, DATA_FLAGS); // DATA 16BITS GDT ENTRY
    gdt_setentry(5, sizeof( task_desc ) - 1,(u32) &task_desc, 0x89, 0x00);
    // For Task State Segment TSS will be implement

    gdt_ptr.limit = sizeof( gdt_entries ) - 1;
    gdt_ptr.base = (u32) &gdt_entries;

    gdt_install( &gdt_ptr );

    printf("[Global Description Table] Initialize - DONE\n");
}

void gdt_setentry(  int index,
                    u32 limit,
                    u32 base,
                    u8 access_byte, 
                    u8 flags
) {
    gdt_entry_t* entry = &gdt_entries[index];
    entry->low_limit = limit & 0xFFFF;
    entry->low_base = base & 0xFFFF;
    entry->mid_base = (base >> 16) & 0xFF;
    entry->high_base = (base >> 24) & 0xFF;

    entry->access_byte = access_byte;
    entry->flags = (flags << 4) | (limit >> 16 & 0x0F);
}
