#include <stdint.h>

void memcpy(void* __dest__, void* __src__, size_t __size__) {
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

void memset(void* __dest__, u8 __src__, size_t __size__ ) {
    u8* dest_tmp = (u8*) __dest__;
    while( __size__ > 0 ) {
        *dest_tmp = __src__;
        dest_tmp++;
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

int strlen(const char* str) {
    int size = 0;
    while( str[size] != 0 )
        size++;
    return size;
}

int strcmp(const char* __cmp1, const char* __cmp2 ) {
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

int strncmp(const char* __cmp1, const char* __cmp2, size_t size ) {
    for(int i = 0; i < size; i++) {
        if( __cmp1[i] != __cmp2[i] )
            return 0;
    }

    return 1;
}

int strreverse(char* buffer) {
    int i = 0, j = strlen( buffer ) - 1;

    for(; i < j; i++,j--) {
        char tmp = buffer[i];
        buffer[i] = buffer[j];
        buffer[j] = tmp;
    }

    return 0;
}