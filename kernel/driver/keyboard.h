#pragma once

#include <stdint.h>

// Based on https://wiki.osdev.org/I8042_PS/2_Controller
#define KEYBOARD_DATA       0x60
#define KEYBOARD_STATUS     0x64
#define KEYBOARD_COMMAND    0x64

enum KeyButton {
    KeyButton_Escape = 0x01,
    KeyButton_1,
    KeyButton_2,
    KeyButton_3,
    KeyButton_4,
    KeyButton_5,
    KeyButton_6,
    KeyButton_7,
    KeyButton_8,
    KeyButton_9,
    KeyButton_0,
    KeyButton_Minus,
    KeyButton_Equal,
    KeyButton_Backspace,
    KeyButton_Tab,
    KeyButton_Q,
    KeyButton_W,
    KeyButton_E,
    KeyButton_R,
    KeyButton_T,
    KeyButton_Y,
    KeyButton_U,
    KeyButton_I,
    KeyButton_O,
    KeyButton_P,
    KeyButton_Open_Backet,
    KeyButton_Close_Bracket,
    KeyButton_Enter,
    KeyButton_Left_Ctrl,
    KeyButton_A,
    KeyButton_S,
    KeyButton_D,
    KeyButton_F,
    KeyButton_G,
    KeyButton_H,
    KeyButton_J,
    KeyButton_K,
    KeyButton_L,
    KeyButton_Semicolumn,
    KeyButton_SingleQuote,
    KeyButton_BackTick,
    KeyButton_Left_Shift,
    KeyButton_ReverSlash,
    KeyButton_Z,
    KeyButton_X,
    KeyButton_C,
    KeyButton_V,
    KeyButton_B,
    KeyButton_N,
    KeyButton_M
};

//typedef void (*__keyboard_callback)(u8 button, u8 scancode, u8 flags) keyboard_callback;

/**
 * Keyboard_Initalize()
 * - Initalize keyboard component with define the Keyboard_Handle on IRQ
 */
void Keyboard_Initialize();

/**
 * Keyboard_AddCall
 * @param keyboard_callback - (void)(u8 key, char scancode, u8 flags)
 */
void Keyboard_AddCall(void* keyboard_callback); 
