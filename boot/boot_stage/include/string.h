#pragma once
#include "stdint.h"

// memory function
void memset(void* __dest__, void* __src__, size_t __size_src__, size_t __size__ );
void memcpy(void* __dest__, void* __src__, size_t __size__ );
void zeromem(void* __dest__, size_t __size__ );

// string function
size_t strlen(const char* buffer);

/**
 * strcmp
 * @return if 1 is same, 0 isnt same
 */
u8 strcmp(const char* __cmp1, const char* __cmp2);
/**
 * strncmp
 * @return if 1 is same, 0 isnt same
 */
u8 strncmp(const char* __cmp1, const char* __cmp2, size_t size);
