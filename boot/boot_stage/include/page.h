#pragma once

#include "stdint.h"

#define MAX_PAGES       0x1000
#define PAGE_ENTRIES    1024

#define PT_INDEX( x ) (x >> 12) & 0x3FF
#define PD_INDEX( x ) x >> 22

#define PAGE_ATTR_PRESENT   1 << 0
#define PAGE_ATTR_READWRITE 1 << 1
#define PAGE_ATTR_USERSPACE 1 << 2

void    PageInitialize();
u32     PageAlloc( );
void    PageMapping( u32 virtual_address, u8 page_flags, size_t size );
u32     PageMappingPhysical( u32 virtual_address, u32 physical_address, u8 page_flags, size_t size );
void    PageTableAttach(u32 address);
void    PageTableUsed();
void    PageCheckAddress(u32 address);