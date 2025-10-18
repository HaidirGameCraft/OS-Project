#pragma once

#include <stdint.h>

/**
 * alloc_initialize()
 * - initalize allocation
 */
void alloc_initialize();

/**
 * malloc()
 * - make the uninitialize location by specific size
 * @return the address of uninitialize location
 */
void* malloc( size_t size );

/**
 * free()
 * - clear all data and make the location as free or available for next malloc
 */
void free( void* ptr );
