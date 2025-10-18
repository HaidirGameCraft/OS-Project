#pragma once

#include <stdint.h>

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
