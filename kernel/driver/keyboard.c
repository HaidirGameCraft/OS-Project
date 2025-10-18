#include <driver/keyboard.h>
#include <cpu/idt.h>
#include <cpu/isr.h>
#include <cpu/port.h>
#include <stdio.h>
#include <driver/video.h>

const char scancode1[256] = {
    0x00, 0x00, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0x08,
    0x0B, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\n', 
    0x00, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`', 0X00, '\\',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', 0x00, '*', 0x00, ' ', 0x00
};

int keyboard_callback_index = 0;
u32 keyboard_callbacks[16] = { 0 };

void Keyboard_Handle( register_t reg );

void Keyboard_Initialize() {
    set_interrupt_function( (u32) &Keyboard_Handle, 33 );
}

void Keyboard_AddCall(void* keyboard_callback) {
    keyboard_callbacks[ keyboard_callback_index++ ] = (u32) keyboard_callback;
}

void Keyboard_Handle( register_t reg ) {
    u8 status = inb( KEYBOARD_STATUS );

    // If the bit 0 is full - it have the output
    if( status & 1 ) {
        u8 data_key = inb( KEYBOARD_DATA );
        u8 flags = 0;
        char scancode = scancode1[ data_key ];

        flags |= ( (data_key >= 0x80) == 1 );

        for(int i = 0; i < 16; i++) {
            if( keyboard_callbacks[i] != 0 ) {
                ((void (*)(u8, char, u8)) keyboard_callbacks[i])(data_key, scancode, flags);
            }
        }
    }
}
