#include <term.h>
#include <stdint.h>
#include <string.h>
#include <driver/video.h>
#include <stdio.h>

void kernel_term( char* command ) {
    if( strcmp(command, "CLEAR") )
    {
        VideoClearScreen(0x00000000);
    }
    else if ( strncmp(command, "PRINT ", strlen("PRINT ") ) )
    {
        printf((const char*) (command + strlen("PRINT ")));
    }
}
