#include <memory.h>
#include <stdio.h>

extern void __heap_start();
extern void __heap_end();
struct __memory_block_t {
    size_t size;
    u8 free;
    struct __memory_block_t* next;
} __attribute__((packed));

typedef struct __memory_block_t memory_block_t;

memory_block_t* heap_start = 0;
memory_block_t* heap_end = 0;

void alloc_initialize() {
    heap_start = (memory_block_t*) __heap_start;
    heap_end = (memory_block_t*) __heap_start;

    heap_start->free = 1;
    heap_start->size = (u32) __heap_end - (u32) __heap_start;
    heap_start->next = NULL;
}

void* malloc( size_t size ) {
    memory_block_t* current = heap_start;
    while( current != 0 ) {
        //printf("%x: Heap Current size %x, Heap required %x\n", (u32) current, current->size, size );
        if( current->free && size + sizeof( memory_block_t ) <= current->size ) {
            u32 remained_size = current->size - size - sizeof( memory_block_t );

            current->free = 0;
            if( size + sizeof( memory_block_t ) == current->size ) {
                return (void*) current + sizeof( memory_block_t );
            }
            
            memory_block_t* new_block = (memory_block_t*) ( (u64)(current) + sizeof( memory_block_t ) + size) ;
            new_block->free = 1;
            new_block->size = remained_size;
            new_block->next = current->next;

            if( new_block->next == NULL )
                heap_end  = new_block;

            current->next = new_block;
            current->size = size + sizeof( memory_block_t );

            return (void*)( (u32)(current) + sizeof( memory_block_t ) );
        }

        current = current->next;
    }

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
        if( block->next == NULL )
            heap_end = block;
    }

    if( prev != 0 && prev->free == 1) {
        prev->size += block->size;
        prev->next = block->next;
        if( prev->next == NULL )
            heap_end = prev;
    }
}

void sbrk(size_t size) {
    //TODO
}