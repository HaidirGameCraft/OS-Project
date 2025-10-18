#pragma once

#include <stdint.h>
#include "../../boot/boot_stage/include/boot_header.h"
#include <string.h>
#include <font.h>

/**
 * InitializeVideo()
 * - Initalize the Video component for rendering ( Graphics )
 */
void InitializeVideo(struct video_mode_header* video_mode_ptr);

/**
 * VideoPutC()
 * - rendering Character
 */
void VideoPutC( char __char );

/**
 * VideoClearScreen()
 * - clear screen with specific color (RGB type)
 */
void VideoClearScreen( u32 color );

/**
 * VideoGetWidth()
 * @return width of screen
 */
u32 VideoGetWidth();

/**
 * VideoGetHeight()
 * @return height of screen
 */
u32 VideoGetHeight();

/**
 * VideoBytesPerPixel()
 * - get how much byte of pixel should be in Graphics
 * @return bytes
 */
u8  VideoBytesPerPixel();

/**
 * VideoGetFramebuffer()
 * @return return the address of framebuffer screen
 */
u32 VideoGetFramebuffer();

/**
 * VideoGetPageTable();
 * @return the address of page table
 */
u32* VideoGetPageTable();