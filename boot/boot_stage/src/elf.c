#include "../include/elf.h"
#include "../include/memory.h"
#include "../include/string.h"
#include "../include/bootlib.h"

ELF32_Header*   ELF_GetHeader( char* buffer ) {
    ELF32_Header* hdr = (ELF32_Header*) malloc( sizeof( ELF32_Header ) );
    memcpy( hdr, buffer, sizeof( ELF32_Header ) );

    // Checking it is ELF format or not
    if( hdr->identifier[0] != 0x7F || strncmp( &hdr->identifier[1], "ELF", strlen("ELF") ) == 0 )
    {
        printf("ELF Error: This is not ELF File format\n");
        free( hdr );
        return 0;
    }

    return hdr;
}

void            ELF_Read( ELF32_Header* header, char* buffer ) {
    ELF32_Program* program_entry = (ELF32_Program*)( (u32)(buffer) + header->pheader_offset );
    u32 pentry_size = header->pheader_entries;

    // Make Mapped first
    for(u32 i = 0; i < pentry_size; i++) {
        ELF32_Program* program = &program_entry[i];
        u32 v_addr = program->p_vaddr;
        u32 p_addr = program->p_paddr;
        //printf("Virtual Address: %x\n", v_addr);
        if( v_addr == 0 )
            continue;
        
        u32 status = 0;
        status = PageMappingPhysical(v_addr, p_addr, PAGE_ATTR_PRESENT | PAGE_ATTR_READWRITE, program->memsize);
        // if( v_addr != p_addr )
        //     status = PageMappingPhysical(v_addr, p_addr, PAGE_ATTR_PRESENT | PAGE_ATTR_READWRITE, program->memsize);
        // else
        //     PageMapping(v_addr, PAGE_ATTR_PRESENT | PAGE_ATTR_READWRITE, program->memsize);
        
        if( status != 0 )
            continue;
          
        //((u32*) v_addr)[0] = 0x11223344;
        memcpy( (void*) v_addr, (buffer + program->p_offset), program->filesize);
        if( program->memsize > program->filesize);
            zeromem( (void*) v_addr, program->memsize - program->filesize);
    }
}

void            ELF_Close(ELF32_Header* header) {
    free( header );
}