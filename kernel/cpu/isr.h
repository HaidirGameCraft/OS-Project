#pragma once
#include <stdint.h>

typedef struct {
    u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
    u32 number_interrupt;
    u32 error_code;
} __attribute__((packed)) register_t;

void isr_handle(register_t reg);
