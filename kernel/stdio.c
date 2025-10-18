#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <driver/video.h>

void print_hexadecimal( u32 value, size_t size ) {
    const char* hexadecimal_words = "0123456789ABCDEF";
    for(int i = 0; i < size; i++) {
        u8 _value = value >> ((size - 1 - i) * 8) & 0xFF;

        VideoPutC(hexadecimal_words[_value >> 4 & 0xF]);
        VideoPutC(hexadecimal_words[_value & 0xF]);
    }
}

void print_binary( u8 value ) 
{
    for(int i = 7; i >= 0; i--)
    {
        VideoPutC( (value >> i & 1) + '0' );
    }
}

void print_integer( int value ) {
    int tmp = value;
    int sign = ( value < 0 ) ? -1 : 1;

    char buffer[20] = {0};
    int i = 0;
    while( tmp > 0 )
    {
        char v = tmp % 10;
        buffer[i++] = '0' + v;
        tmp /= 10;
    }

    if( sign == -1 )
        buffer[i++] = '-';
    
    buffer[i] = 0;
    strreverse( buffer );
    print( buffer );
}

void print(const char* text) {
    int index = 0;
    while( text[index] != 0 ) {
        VideoPutC( text[index] );
        index++;
    }
}

void printf(const char* fmt, ...) {
    u32* arg = (u32*)( &fmt + 1 );

    int index = 0;
    while( fmt[ index ] != 0 )
    {
        if( fmt[ index ] == '%' )
        {
            if( fmt[ index + 1] == 's' )
            {
                printf( (char*) *arg++ );
                index++;
            }
            else if ( fmt[index + 1] == 'x' )
            {
                print_hexadecimal( (u32) *arg++ , 4 );
                index++;
            }
            else if ( fmt[index + 1] == 'b' )
            {
                print_binary( (u8) *arg++ );
                index++;
            }
            else if ( fmt[index + 1] == 'i' )
            {
                print_integer( (int) *arg++ );
                index++;
            }
            index++;
        }
        else {
            VideoPutC( fmt[index++] );
        }
    }
}
