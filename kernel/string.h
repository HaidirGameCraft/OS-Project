#pragma once

#include <stdint.h>

/**
 * memcpy()
 * - copy the source address to destination address with specific size
 */
void memcpy(void* __dest__, void* __src__, size_t __size__);

/**
 * memset()
 * - set the source value to destination with specific size
 */
void memset(void* __dest__, u8 __src__, size_t __size__);

/**
 * zeromem()
 * - make the destination containing zero
 */
void zeromem(void* __dest__, size_t __size__ );


/**
 * strlen()
 * - give the length of text
 */
int strlen(const char* str);

/**
 * strcmp()
 * - comparing cmp1 with cmp2
 * @return 0 = NO, 1 = YES
 */
int strcmp(const char* __cmp1, const char* __cmp2 );

/**
 * strncmp()
 * - comparing cmp1 and cmp2 with specific size
 * @return 0 = NO, 1 = YES
 */
int strncmp(const char* __cmp1, const char* __cmp2, size_t size );

/**
 * strreverse()
 * - reverse location of buffer
 */
int strreverse(char* buffer);
