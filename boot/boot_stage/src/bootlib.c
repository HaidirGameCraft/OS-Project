#include "../include/bootlib.h"
#include "../include/video.h"
#include "../include/string.h"
#include "../include/page.h"
#include "../include/cpu.h"

void Initialize(struct boot_header* bootheader) {
    InitializeVideo( (struct video_mode_header*) bootheader->video_mode_ptr);
    PageInitialize();

    gdt_initialize();
    idt_initialize();

    MemoryInitialize();
    //initializeDisk();
}

char* hexawords = "0123456789ABCDEF";
void printhexa( u32 value ) {
    for(int i = 0; i < 4; i++)
    {
        u8 vbyte = ( value >> ((3 - i) * 8)) & 0xFF;
        VideoPutC( hexawords[ vbyte >> 4 & 0x0F ] );
        VideoPutC( hexawords[ vbyte & 0x0F] );
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
                printhexa( (u32) *arg++ );
                index++;
            }
            index++;
        }
        else {
            VideoPutC( fmt[index++] );
        }
    }
}