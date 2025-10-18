ENTRY(__start)
SECTIONS
{
    . = 0x100000;
    __kernel_start = .;
    .text : ALIGN(4K) {
        *(.text)
    }
    .rodata : ALIGN(4K) {
        *(.rodata)
    }
    .data : ALIGN(4K) {
        *(.data)
    }
    .bss : ALIGN(4K) {
        *(COMMON)
        *(.bss)
    }
    . = ALIGN(4);
    __heap_start = .;
    . += 0x40000;
    __heap_end = .;
    __kernel_end = .;
}
