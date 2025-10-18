#pragma once

#include "stdint.h"
#include "boot_header.h"
#include "string.h"

void InitializeVideo(struct video_mode_header* video_mode_ptr);
void VideoPutC( char __char );
void VideoClearScreen( u32 color );
struct video_mode_header* VideoGetModeDetails();
u32 VideoGetWidth();
u32 VideoGetHeight();
u8  VideoBytesPerPixel();
u32 VideoGetFramebuffer();
u32* VideoGetPageTable();