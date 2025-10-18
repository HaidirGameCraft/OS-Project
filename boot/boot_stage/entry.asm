[bits 32]
section .boot_header
global boot_header
boot_header:
magic_number:   dd 0x3B002D1F
entry_start:    dd __start
flags:          dd 0x00000000
video_mode_ptr: dd 0x00000000

section .text
__start:    mov eax, [ esp + 4 ]
            mov esp, __stack_bottom
                    
            [extern main_boot]
            push eax
            call main_boot
            add esp, 4
            
            hlt
            jmp $

section .bss
__stack_top:    resb 0x4000
__stack_bottom:
