SECTIONS
{
    . = 0x8000;
    .text : {
        *(.starter)
        *(.text)
    }
    .rodata : {
        *(.rodata)
    }
    .data : {
        *(.data)
    }
    .bss : {
        *(.bss)
    }
}