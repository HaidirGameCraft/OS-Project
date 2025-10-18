#pragma once

#include "stdint.h"

/**
 * ELF Header
 * Referrence: https://wiki.osdev.org/ELF
 * 
 * ELF ( Executable and Linkable Format )
 */

typedef struct {
    u8  identifier[16];
    u16  type;
    u16  instruction;
    u32 version;
    u32 pentry;
    u32 pheader_offset;
    u32 sheader_offset;
    u32 flags;
    u16 hsize;
    u16 pheader_size;
    u16 pheader_entries;
    u16 sheader_size;
    u16 sheader_entries;
    u16 sstr_index;
} __attribute__((packed)) ELF32_Header;

typedef struct {
    u32 type;
    u32 p_offset;
    u32 p_vaddr;
    u32 p_paddr;
    u32 filesize;
    u32 memsize;
    u32 flags;
    u32 p_align;
} __attribute__((packed)) ELF32_Program;

ELF32_Header*   ELF_GetHeader( char* buffer );
void            ELF_Read( ELF32_Header* header, char* buffer );
void            ELF_Close(ELF32_Header* header);