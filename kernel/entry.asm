[bits 32]
; section .multiboot
; align 4
; MULTIBOOT_PAGE_ALIGN    equ 1 << 0
; MULTIBOOT_MEMORY_INFO   equ 1 << 1
; MULTIBOOT_VIDEO_MODE    equ 1 << 2
; MULTIBOOT_FLAGS         equ 0
; multiboot_header_start: dd 0x1BADB002   ; MAGIC NUMBER
;                         dd MULTIBOOT_FLAGS   ; FLAGS
;                         dd -(0x1BADB002 + MULTIBOOT_FLAGS); CHECKSUM

;                         ; Video Mode Configuration
section .text
global __start
__start: mov eax, [ esp + 4 ]
        mov esp, _stack_bottom_
        ; Calling kernel_main
        [extern kernel_main]
        push eax
        call kernel_main

        hlt
        jmp $

section .bss
_stack_top_:    resb 8190
_stack_bottom_:
