ENTRY(__start)
SECTIONS
{
    . = 0xC0100000;
    __kernel_start = .;
    .text : AT(ADDR(.text) - 0xC0000000) ALIGN(4K) {
        *(.text)
    }
    .rodata : AT(ADDR(.rodata) - 0xC0000000) ALIGN(4K) {
        *(.rodata)
    }
    .data : AT(ADDR(.data) - 0xC0000000) ALIGN(4K) {
        *(.data)
    }
    .bss : AT(ADDR(.bss) - 0xC0000000) ALIGN(4K) {
        *(COMMON)
        *(.bss)
    }
    . = ALIGN(4);
    __heap_start = .;
    . += 0x40000;
    __heap_end = .;
    __kernel_end = .;
}
