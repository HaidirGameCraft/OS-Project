#include "include/stdint.h"
#include "include/video.h"
#include "include/bootlib.h"
#include "include/elf.h"

extern void __boot_start__();
extern void __boot_end__();

extern void boot_header();
void main_boot(struct boot_header *bootheader) {
    Initialize( bootheader );
    //VideoClearScreen(0x00000000);
    printf("Hello, World\n");
    printf("HaOS Version: 0x%x\n", 0x10000001 );

    printf("Boot Start Address: 0x%x\n", (u32) __boot_start__);
    printf("Boot End Address: 0x%x\n", (u32) __boot_end__);

    BootDiskInfo bootdiskinfo = GetDiskInfo();
    VolumeInfo volume_info = OpenVolume( &bootdiskinfo, 1 );
    FATDirectory kernelfile = FAT_FindFile( &volume_info, "KERNEL  ELF");

    char* buffer = (char*) malloc( kernelfile.filesize );
    FAT_ReadFile( &volume_info, &kernelfile, buffer );

    ELF32_Header *elf_header = ELF_GetHeader( buffer );
    ELF_Read( elf_header, buffer );
    
    void (*_entry)(struct boot_header*) = (void (*)(struct boot_header*)) ((u32) elf_header->pentry);
    
    ELF_Close( elf_header );
    free( buffer );

    //PageTableAttach(bootheader->video_mode_ptr & ~0xFFF);
    PageMappingPhysical(bootheader->video_mode_ptr & ~0xFFF, bootheader->video_mode_ptr & ~0xFFF, PAGE_ATTR_PRESENT | PAGE_ATTR_READWRITE, 0);
    //PageTableUsed();
    //PageCheckAddress(0x8000);

    printf("Done at %x\n", bootheader->video_mode_ptr );

    bootheader->video_mode_ptr = (u32) VideoGetModeDetails();
    _entry(bootheader);
}
