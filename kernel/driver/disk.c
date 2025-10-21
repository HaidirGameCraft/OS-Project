#include "disk.h"
#include <cpu/port.h>
#include <memory.h>
#include <stdio.h>

/**
 * ATA_ReadDisk()
 * - read data from disk
 */
void ATA_ReadDisk(u32 LBA, u16 drive, char* buffer, size_t size)
{

    int size_length = (int)( size / 512 );
    if( size % 512 != 0 )
        size_length += 1;
        
    u16* tbuf = (u16*) malloc( 512 );
    u32 ibuffer = 0;
    for(int i = 0; i < size_length; i++) {
        // set the ATA Master with LBA 24 bits to DriveReg
        u32 tLBA = LBA + i;
        outb( drive + ATA_DriveReg, ATA_Master | ( (tLBA >> 24) & 0x0F ) );

        outb( drive + ATA_SectorCountReg, 1 );
        outb( drive + ATA_LBALowReg, (u8) ( tLBA & 0xFF ) );
        outb( drive + ATA_LBAMidReg, (u8) ((tLBA >> 8) & 0xFF));
        outb( drive + ATA_LBAHighReg, (u8) ((tLBA >> 16) & 0xFF));
        
        // Set the command read based on matrix command
        outb( drive + ATA_CommandReg, ATA_ReadSector);
        
        // // Waiting for the drive service
        while( (inb( drive + ATA_StatusReg ) & ATA_BSY ) );

        for(int j = 0; j < 256; j++) {
            if( i == size_length - 1 )
            {
                tbuf[j] = inw( drive | ATA_DataReg );
            }
            else {
                ((u16*)( (u32) buffer + ibuffer ))[0] = inw( drive | ATA_DataReg );
                ibuffer += 2;
            }

        }

        
        if( i == size_length - 1 )
        {
            int size_ = size % 512;
            if( size_ == 0 )
                size_ = 512;

            for(int j = 0; j < size_; j++ )
            {
                ((u8*)((u32) buffer + ibuffer))[j] = ((u8*) tbuf)[j];
            }
        }
        
        
    }

    free( tbuf );
}

/**
 * ATA_WriteDisk()
 * - write data to disk
 */
void ATA_WriteDisk(u32 LBA, u16 drive, char* buffer, size_t size) {
    // STUB
}