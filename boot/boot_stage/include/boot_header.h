#pragma once

#include "stdint.h"

/**
 * Boot Header
 */

struct boot_header {
    u32 magic_number;
    u32 entry_start;
    u32 flags;
    u32 video_mode_ptr;
} __attribute__((packed));

struct boot_flags_video_mode {
    u16 width;
    u16 height;
    u16 bits_per_pixel;
    u16 reserved;
} __attribute__((packed));

/**
 * Video Mode Header
 * Based On: https://wiki.osdev.org/VESA_Video_Modes
 */

struct video_mode_header {
    u16 attributes;
    u8  first_window;
    u8  second_window;
    u16 granularity;
    u16 window_size;
    u16 first_segment;
    u16 second_segment;
    addr_t window_function_ptr;
    u16 pitch;
    u16 width;
    u16 height;
    
    u8  width_char;
    u8  height_char;
    u8  planes;
    u8  bits_per_pixel;
    u8  banks;
    u8  memory_model;
    u8  back_size;
    u8  images_pages;
    u8  reserved0;

    u8  red_mask;
    u8  red_position;
    u8  green_mask;
    u8  green_position;
    u8  blue_mask;
    u8  blue_position;
    u8  reserved_mask;
    u8  reserved_position;
    u8  direct_color_attributes;

    addr_t framebuffer;
    u32 offscreen_memory_offset;
    u16 offscreen_memory_size;
} __attribute__((packed));

struct boot2_header {
    u32 magic_number;
    u32 entry_start;
    u32 flags;
    u32 video_mode_ptr;
    u32 partition_ptr;
} __attribute__((packed));

struct boot_header_2 {
    u32 magic_number;
    u32 entry_start;
    u32 flags;
    struct video_mode_header video_mode_ptr;
};

#define BOOT_FLAGS_VIDEO_MODE   (1 << 1)
#define HALT    for(;;){ __asm__ volatile("cli; hlt"); }