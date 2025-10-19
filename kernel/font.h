#pragma once

#include "stdint.h"
#define TOTAL_CHARS_BITMAP 95

#define CHAR_WIDTH_PIXEL 8
#define CHAR_HEIGHT_PIXEL 16

/**
 * font_getcharbitmap()
 * - get the bitmap
 */
const u8* font_getcharbitmap(char __c);