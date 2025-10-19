#include <stdio.h>
#include <cpu/gdt.h>
#include <cpu/idt.h>
#include <driver/video.h>
#include <driver/keyboard.h>
#include <memory.h>
#include <page.h>
#include <term.h>
#include <kernel.h>
#include "../boot/boot_stage/include/boot_header.h"

int log_buffer_index = 0;
char log_buffer[1024] = {0};
void keyboard_callback(u8 button, char scancode, u8 flags);

// the entry point after callig from entry.asm
void kernel_main( struct boot_header* bootheader ) {

    struct video_mode_header* mb = (struct video_mode_header*) bootheader->video_mode_ptr;

    InitializeVideo( (struct video_mode_header*) bootheader->video_mode_ptr);
    VideoClearScreen(0x00000000);
    page_initialize();
    gdt_initialize();
    idt_initialize();
    alloc_initialize();
    

    Keyboard_AddCall( keyboard_callback );

    printf("OS Project\n Creator By: Haidir\n");
    printf("KERNEL START: 0x%x\nKERNEL END: 0x%x\n", (u32) __kernel_start, (u32) __kernel_end);

    return;
};

void keyboard_callback(u8 button, char scancode, u8 flags) {
    //printf("Scancode: %x\n", scancode);
    if ( flags & 0b1 )
    return;
    
    // When user press the Enter button executin command
    if( scancode == 0x00 )
    return;
    

    if( scancode == '\n' )
    {
        VideoPutC('\n');
        kernel_term( log_buffer );
        log_buffer_index = 0;
        log_buffer[log_buffer_index] = 0;
        printf("/> ");
    }
    else if ( button == KeyButton_Backspace ) {
        if( log_buffer_index <= 0 )
            return;

        VideoPutC( scancode );
        log_buffer[ log_buffer_index-- ] = 0;
        log_buffer[ log_buffer_index ] = 0;
    }
    else {
        VideoPutC( scancode );
        log_buffer[ log_buffer_index++ ] = scancode;
        log_buffer[ log_buffer_index ] = 0;
    }
}
