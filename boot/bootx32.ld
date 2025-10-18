SECTIONS
{
    . = 0x10000;
    __boot_start__ = .;
    .boot_header : {
        *(.boot_header)
    }
    
    .boot_stage : {
        *(.boot_stage)
    }
    .boot_stage.data : {
        *(.boot_stage.data)
    }

    .text : {
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
    . = ALIGN(4);
    __boot_end__ = .;
    
}
