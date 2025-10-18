#include "../include/video.h"
#include "../include/font.h"
#include "../include/page.h"
struct video_mode_header video_mode;
u32 max_char_width = 0;
u32 max_char_height = 0;
u32 text_lines = 0;
u32 text_columns = 0;
u8* framebuffer = 0;

u32 video_page_table[ PAGE_ENTRIES ] __attribute__((aligned( MAX_PAGES )));
void VideoScroll();

struct video_mode_header* VideoGetModeDetails() {
    return &video_mode;
}

void InitializeVideo(struct video_mode_header* video_mode_ptr) {
    memcpy( &video_mode, video_mode_ptr, sizeof( struct video_mode_header ) );

    max_char_width = video_mode_ptr->width / 8;
    max_char_height = video_mode_ptr->height / 16;
    framebuffer = (u8*) video_mode_ptr->framebuffer;

    text_lines = 0;
    text_columns = 0;

}

void VideoPutC( char __char ) {
    u32 index = text_columns + text_lines * max_char_width;

    if( __char == 0x00 )
    {
        return;
    }
    else if( __char == '\n' )
    {
        text_lines++;
        text_columns = 0;
        index = text_columns + text_lines * max_char_width;
        text_columns = index % max_char_width;
        text_lines = (u32)( index / max_char_width);
        VideoScroll();
        return;
    }

    const u8* char_bitmap = font_getcharbitmap( __char );
    for(int i = 0; i < 16; i++)
    {
        u8 bit = char_bitmap[i];
        for(int j = 0; j < 8; j++) {
            int index = ((text_columns * 8 + j) + ( text_lines * 16 + i ) * video_mode.width) * video_mode.bits_per_pixel / 8;
            
            if( (bit >> (8 - j - 1)) & 0b1 )
            {
                framebuffer[index + 0] = 0xFF;
                framebuffer[index + 1] = 0xFF;
                framebuffer[index + 2] = 0xFF;
            } else {
                framebuffer[index + 0] = 0x00;
                framebuffer[index + 1] = 0x00;
                framebuffer[index + 2] = 0x00;
            }
        }
    }

    text_columns++;
    index = text_columns + text_lines * max_char_width;
    text_columns = index % max_char_width;
    text_lines = (u32)( index / max_char_width);
    VideoScroll();
}

void VideoScroll() {
    if( text_lines < max_char_height )
        return;

    for(int i = 0; i < max_char_height - 1; i++)
    {
        memcpy(&framebuffer[i * video_mode.width * 16 * ( video_mode.bits_per_pixel / 8 )], &framebuffer[(i + 1) * video_mode.width * 16 * ( video_mode.bits_per_pixel / 8 )], video_mode.width * 16 * ( video_mode.bits_per_pixel / 8 ));
    }

    zeromem(&framebuffer[(max_char_height - 1) * video_mode.width * 16 * ( video_mode.bits_per_pixel / 8 )], video_mode.width * 16 * ( video_mode.bits_per_pixel / 8 ));
    text_lines--;
    u32 index = text_columns + text_lines * max_char_width;
    text_columns = index % max_char_width;
    text_lines = (u32)( index / max_char_width);
}

u32 VideoGetWidth() {
    return video_mode.width;
}
u32 VideoGetHeight() {
    return video_mode.height;
}
u8  VideoBytesPerPixel() {
    return (video_mode.bits_per_pixel / 8);
}

void VideoClearScreen( u32 color ) {
    for(int y = 0; y < video_mode.height; y++) {
        for(int x = 0; x < video_mode.width; x++) {
            u32 index = (x + y * video_mode.width) * ( video_mode.bits_per_pixel / 8 );

            framebuffer[ index + 0 ] = color & 0xFF;
            framebuffer[ index + 1 ] = ( color >> 8 ) & 0xFF;
            framebuffer[ index + 2 ] = ( color >> 16 ) & 0xFF;
        }
    }
}
u32 VideoGetFramebuffer() {
    return (u32) framebuffer;
}
u32* VideoGetPageTable() {
    return video_page_table;
}