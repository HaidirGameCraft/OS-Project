#pragma once

/**
 * ATAIO Disk
 * Based On: https://wiki.osdev.org/ATA_PIO_Mode
 * Command Matrix PIO: https://wiki.osdev.org/ATA_Command_Matrix
 */

#include <stdint.h>

#define ATA_PrimaryBus      0x1F0
#define ATA_SecondaryBus    0x170

#define ATA_Master          0xE0
#define ATA_Slave           0xF0

/**
 * Register of ATA
 */
#define ATA_DataReg         0x00
#define ATA_ErrorReg        0x01
#define ATA_FeatureReg      0x01
#define ATA_SectorCountReg  0x02

// ATA CHS
#define ATA_SectorNumReg    0x03
#define ATA_CylLowReg       0x04
#define ATA_CylHighReg      0x05

// ATA LBA
#define ATA_LBALowReg       0x03
#define ATA_LBAMidReg       0x04
#define ATA_LBAHighReg      0x05

#define ATA_DriveReg        0x06
#define ATA_HeadReg         0x06

#define ATA_StatusReg       0x07
#define ATA_CommandReg      0x07

// Error Register Status
#define ATA_AMNF            1 << 0  // Address mark is not found
#define ATA_TKZNF           1 << 1  // Track zero not found
#define ATA_ABRT            1 << 2  // Aborted command
#define ATA_MCR             1 << 3  // Media change request
#define ATA_IDNF            1 << 4  // ID not found
#define ATA_MC              1 << 5  // Media changed
#define ATA_UNC             1 << 6  // Uncorrectable Data Error
#define ATA_BBK             1 << 7  // Bad Block Detected

// Status Register
#define ATA_ERR             1 << 0  // Error Occurred
#define ATA_IDX             1 << 1  // Index
#define ATA_CORR            1 << 2  // Corrected Data
#define ATA_DRQ             1 << 3  // Driver has PIO Data Transfer
#define ATA_SRV             1 << 4  // Overlapped Mode Service Request
#define ATA_DF              1 << 5  // Drive Fault Error
#define ATA_RDY             1 << 6  // Drive os spun down or after an error
#define ATA_BSY             1 << 7  // Preparing receive/send

/**
 * ATA Commanc Matrix
 */

#define ATA_NOP             0x00
#define ATA_ReadSector      0x20
#define ATA_WriteSector     0x30


/**
 * ATA_ReadDisk()
 * - read data from disk
 */
void ATA_ReadDisk(u32 LBA, u16 drive, char* buffer, size_t size);

/**
 * ATA_WriteDisk()
 * - write data to disk
 */
void ATA_WriteDisk(u32 LBA, u16 drive, char* buffer, size_t size);
