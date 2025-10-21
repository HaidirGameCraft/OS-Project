#include "../include/memory.h"
#include "../include/page.h"
#include "../include/bootlib.h"

extern void __boot_end__();

struct __memory_block_t {
    size_t size;
    u8 free;
    struct __memory_block_t* next;
} __attribute__((packed));
typedef struct __memory_block_t memory_block_t;

memory_block_t* heap_start = 0;
memory_block_t* heap_end = 0;

void MemoryInitialize() {
    heap_start = (memory_block_t*) __boot_end__;
    heap_end = (memory_block_t*)( (u32) __boot_end__ + 0x10000);
    PageMapping( (u32) heap_start, PAGE_ATTR_PRESENT | PAGE_ATTR_READWRITE, 0x10000, 0);

    heap_start->free = 1;
    heap_start->size = 0x10000;
    heap_start->next = 0;
}

void* malloc( size_t size ) {
    memory_block_t* current = heap_start;
    memory_block_t* last = 0;
    while( current != 0 ) {
        printf("%x: Heap Current size %x, Heap required %x\n", (u32) current, current->size, size );
        if( current->free && size + sizeof( memory_block_t ) <= current->size ) {
            u32 remained_size = current->size - size - sizeof( memory_block_t );

            current->free = 0;
            if( size + sizeof( memory_block_t ) == current->size ) {
                return (void*) current + sizeof( memory_block_t );
            }
            
            memory_block_t* new_block = (memory_block_t*) ( (u32)(current) + sizeof( memory_block_t ) + size) ;
            new_block->free = 1;
            new_block->size = remained_size;
            new_block->next = current->next;

            current->next = new_block;
            current->size = size + sizeof( memory_block_t );

            return (void*)( (u32)(current) + sizeof( memory_block_t ) );
        }

        last = current;
        current = current->next;
    }

    // Expand Malloc memory
    printf("Expand\n");
    u32 remaining = (size + sizeof( memory_block_t ) ) - last->size;
    u32 requried_size = (u32)( remaining / MAX_PAGES ) + 1;
    PageMapping( (u32) heap_end, PAGE_ATTR_PRESENT | PAGE_ATTR_READWRITE , requried_size * MAX_PAGES, 0);
    //printf("Expanded Malloc Memory %x + %x = %x\n", (u32) last, last->size, (u32) last + last->size );
    heap_end = (memory_block_t*) ((u32)( heap_end ) + requried_size + MAX_PAGES );

    if( last->free == 0 )
    {
        current = (memory_block_t*)( (u32)(last) + last->size );
        current->free = 1;
        current->size = requried_size * MAX_PAGES;
        current->next = 0;

        last->next = current;
    }
    else {
        last->size += requried_size * MAX_PAGES;
        current = last;
    }

    if( current->free && size + sizeof( memory_block_t ) <= current->size ) {
        u32 remained_size = current->size - size - sizeof( memory_block_t );

        current->free = 0;
        if( size + sizeof( memory_block_t ) == current->size ) {
            return (void*) current + sizeof( memory_block_t );
        }
        
        memory_block_t* new_block = (memory_block_t*) ( (u32)(current) + sizeof( memory_block_t ) + size) ;
        new_block->free = 1;
        new_block->size = remained_size;
        new_block->next = current->next;
        
        current->next = new_block;
        current->size = size + sizeof( memory_block_t );
        return (void*)( (u32)(current) + sizeof( memory_block_t ) );
    }

    printf("Memory Failed: Out Of Range\n");
    return 0;
}

void free( void* ptr ) {
    if( !ptr )
        return;

    memory_block_t* block = (memory_block_t*)( (u32)(ptr) - sizeof( memory_block_t ) );
    block->free = 1;

    memory_block_t* next = block->next;
    memory_block_t* prev = 0;

    memory_block_t* crt = heap_start;
    if( crt != block )
    {
        while( crt->next != 0 )
        {
            if( crt->next == block )
            {
                prev = crt;
                break;
            }
            crt = crt->next;
        }
    }

    if( next != 0 && next->free == 1)
    {
        block->size += next->size;
        block->next = next->next;
    }

    if( prev != 0 && prev->free == 1) {
        prev->size += block->size;
        prev->next = block->next;
    }
}
