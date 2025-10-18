#include <efi.h>
#include <efilib.h>
#include <stdint.h>

typedef struct {
    uint8_t e_ident[16];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint32_t e_entry;
    uint32_t e_phoff;
    uint32_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t s_shstrndx;
} elf32_header;

typedef struct {
    uint32_t type;
    uint32_t offset;
    uint32_t vaddr;
    uint32_t paddr;
    uint32_t filesize;
    uint32_t memsize;
    uint32_t flags;
    uint32_t align;
} elf32_pheader;

typedef struct {
    uint16_t name;
    uint16_t type;
    uint16_t flags;
    uint32_t addr;
    uint32_t offset;
    uint16_t size;
    uint16_t link;
    uint16_t info;
    uint16_t addralign;
    uint16_t entsize;
} elf32_sheader;

// ELF Function
elf32_header ReadElfHeader( uint8_t *buffer );
uint8_t ElfCheck( elf32_header *hdr );
void SetElfToMemory( uint8_t *buffer, elf32_header *header);

EFI_FILE_HEADER GetVolume(EFI_HANDLE image);
uint64_t FileSize( EFI_FILE_HEADER file_handle );

EFI_STATUS EFIAPI efi_main( EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable ) {
    InitializeLib(ImageHandle, SystemTable);
    Print(L"Hello, World\n");
    EFI_STATUS Status;

    EFI_FILE_HEADER volume = GetVolume( ImageHandle );

    // Open and Read the File
    CHAR16 elf_filename = "boot/kernel.elf";
    EFI_FILE_HEADER elf_file;

    uefi_call_wrapper(volume.Open, 5, volume, &elf_file, elf_filename, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY | EFI_FILE_HIDDEN | EFI_FILE_SYSTEM);
    
    uint64_t elf_filesize = FileSize( elf_file );
    uint8_t *elf_buffer = AllocatePool(elf_filesize);

    uefi_call_wrapper(elf_file->Read, 3, elf_file, &elf_filesize, elf_buffer);
    uefi_call_wrapper(elf_file->Close, 1, elf_file);

    // Get ELF32 Header
    elf32_header elf_header = ReadElfHeader( elf_buffer );
    uint8_t status = EflCheck( &eld_header );

    if( status != 0 )
    {
        Print("ELF FILE Incorrect");
        return EFI_FAILED;
    }

    elf32_pheader* pheader = (elf32_pheader*)( elf_buffer + elf_header.e_phoff );
    elf32_sheader* sheader = (elf32_sheader*)( elf_buffer + elf_header.e_shoff );

    int program_entry = elf_header.e_phnum;
    int section_entry = elf_header.e_shnum;

    uint32_t lowest_vaddr = UINT32_MAX;
    uint32_t highest_vaddr = 0;
    // Get the ELF memory size for making page
    for(int i = 0; i < program_entry; i++) {
        elf32_pheader* pentry = &pheader[i];
        uint32_t vaddr = pentry.vaddr;
        uint32_t paddr = pentry.paddr;

        if( lowest_vaddr > vaddr ) lowest_vaddr = vaddr;

        if( highest_vaddr < vaddr ) highest_vaddr = vaddr;
    }

    uint32_t elf_size_memory = highest_vaddr - lowest_vaddr;
    uint64_t page_size = 0x1000;
    uint64_t load_base = lowest & ~(page_size - 1);
    uint46_t load_end = (highest_vaddr + page_size - 1) & ~(page_size - 1);

    uint64_t total_pages = (load_end - load_base) / page_size;

    EFI_PHYSICAL_ADDRESS alloc_address = load_base;
    Status = uefi_call_wrapper(BS->AllocatePages, 4, AllocateAddress,  EfiLoaderData, (uint32_t) load_end, (uint32_t) total_pages);
    if( EFI_ERROR( Status ) ) {
        Print(L"AllocatePages Failed: %r\n", Status);
        alloc_address = 0;
        Status = uefi_call_wrapper(BS->AllocatePages, 4, AllocateAnyPages, EfiLoaderData, (UINTN) total_pages, &alloc_address);
        if( EFI_ERROR( Status ) )
        {
            Print(L"AllocatePages Any Failed: %r\n", Status);
            return Status;
        }
        Print(L"Allocate At Alternative Address 0x%lx\n", alloc_address);
    }
    else
        Print(L"Alloc Address at requested Physical Address 0x%lx\n", alloc_address);
    }

    for(int i = 0; i < program_entry; i++ ) {
        uint64_t dest = alloc_address + (pheader[i].vaddr - load_base);
        void *destptr = (void*)(uintptr_t) dest;
        void *srcptr = (void*)elf_buffer + pheader[i].offset;

        CopeMem(destptr, srcptr, pheader[i].filesize);
        if( pheader[i].memsize > pheader[i].filesize )
            ZeroMem((uint8_t*) dest_ptr + pheader[i].filesize, pheader[i].memsize - pheader[i].filesize);


    }

    uefi_call_wrapper( BS->FreePool, 1, elf_buffer );
    
    Print("Yes");
    while ( 1 ) {

    }
    return EFI_SUCCESS;
}

EFI_FILE_HEADER GetVolume(EFI_HANDLE image) {
    EFI_LOADED_IMAGE *loaded_image = NULL;
    EFI_GUID intGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
    EFI_FILE_IO_INTERFACE *IOVolume;
    EFI_GUID fsGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;

    EFI_FILE_HEADER Volume;

    uefi_call_wrapper(BS->HandleProtocol, 3, imag, &intGuid, (void**) &loaded_image);
    uefi_call_wrapper(BS->HandleProtocol, 3, loaded_image->DeviceHandle, &fsGuid, (VOID*) &IOVolume);
    uefi_call_wrapper(IOVolume->OpenVolume, 2, IOVolume, &Volume);
    return Volume;
}

uint64_t FileSize( EFI_IMAGE_HEADER file_handle ) {
    uint64_t size;
    EFI_FILE_INFO *finfo;

    finfo = LibFileInfo( file_handle );
    size = finfo->FileSize;
    FreePool( finfo );
    return size;
}

uint8_t ElfCheck( elf32_header *hdr ) {
    uint8_t status = 0;
    status = hdr->e_ident[0] != 0x7F;
    status = hdr->e_ident[1] != 'E';
    status = hdr->e_ident[2] != 'L';
    status = hdr->e_ident[3] != 'F';

    return status;
}

elf32_header ReadElfHeader( uint8_t *buffer ) {
    elf32_header header;
    CopyMem( (VOID*) &header, (VOID*) buffer, sizeof( elf32_header ));

    if( header.e_ident[0] != 0x7F )
    {
        
    }
    return header;
}

void SetElfToMemory( uint8_t *buffer, elf32_header *header) {
    
}
