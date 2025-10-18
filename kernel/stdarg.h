#pragma once
#include <stdint.h>

struct va_argv_list {
    u32* stack;
    u32 old_tmp;
    int index;
};

#define argv_start(v, x)    v.stack = 0; \
                            v.old_tmp = 0; \
                            v.index = 0; \
                            v.stack = (u32*)(&x)

#define argv_end(v)         v.stack = 0;
#define argv_get(v, x)      (x) v.stack[v.index++]
