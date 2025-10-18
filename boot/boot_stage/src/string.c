#include "../include/string.h"

void memset(void* __dest__, void* __src__, size_t __size_src__, size_t __size__ ) {
    if( __size_src__ > __size__ )
        __size_src__ = __size__;

    int src_index = 0;
    int dest_index = 0;
    u8* __dest8 = (u8*) __dest__;
    u8* __src8 = (u8*) __src__;

    while( __size__ > 0 )
    {
        *((u8*)((u32) __dest8 + dest_index)) = *((u8*)((u32) __dest8 + src_index));
        dest_index++;
        src_index = (src_index + 1) % __size_src__;
        __size__--;
    }

}

void memcpy(void* __dest__, void* __src__, size_t __size__ ) {
    u32* dest_tmp = (u32*) __dest__;
    u32* src_tmp = (u32*) __src__;

    // try copy the src to dest using u32 size first
    while( __size__ >= sizeof( u32 ) ) {
        *dest_tmp = *src_tmp;
        dest_tmp++;
        src_tmp++;
        __size__ -= sizeof( u32 );
    }

    // if there have remainder use u8 size for copy
    u8* dest_u8 = (u8*) dest_tmp;
    u8* src_u8 = (u8*) src_tmp;
    while( __size__ > 0 ) {
        *dest_u8 = *src_u8;
        dest_u8++;
        src_u8++;
        __size__--;
    }
}

void zeromem(void* __dest__, size_t __size__ ) {
    u32* dest_tmp = (u32*) __dest__;

    // try copy the src to dest using u32 size first
    while( __size__ >= sizeof( u32 ) ) {
        *dest_tmp = 0;
        dest_tmp++;
        __size__ -= sizeof( u32 );
    }

    // if there have remainder use u8 size for copy
    u8* dest_u8 = (u8*) dest_tmp;
    while( __size__ > 0 ) {
        *dest_u8 = 0;
        dest_u8++;
        __size__--;
    }
}

size_t strlen(const char* str) {
    int size = 0;
    while( str[size] != 0 )
        size++;
    return size;
}

u8 strcmp(const char* __cmp1, const char* __cmp2 ) {
    int __size1 = strlen( __cmp1 );
    int __size2 = strlen( __cmp2 );

    // Returning 0 when __size1 is not same as __size2
    if( __size1 != __size2 )
        return 0;

    for(int i = 0; i < __size1; i++) {
        // Returning 0 when __cmp1 and __cmp2 is not same character
        if( __cmp1[i] != __cmp2[i] )
            return 0;
    }

    return 1;
}

u8 strncmp(const char* __cmp1, const char* __cmp2, size_t size ) {
    for(int i = 0; i < size; i++) {
        if( __cmp1[i] != __cmp2[i] )
            return 0;
    }

    return 1;
}
