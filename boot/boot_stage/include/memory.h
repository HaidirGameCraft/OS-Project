#pragma once

#include "stdint.h"

void    MemoryInitialize();
void*   malloc( size_t __size__ );
void    free( void* __ptr__ );