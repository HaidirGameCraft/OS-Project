[bits 16]
section .starter
jmp __load_start
boot_configuration:
.video_mode:
.width:         dw 800
.height:        dw 600
.bpp:           dw 16

global boot_table
boot_table:     dw 0    ; Partition Offset
                dd 0    ; LBA Start Partition

%include "boot/print.asm"
__load_start:   mov ax, 0
                mov ds, ax
                mov es, ax 
                mov ss, ax

                pop si
                mov ax, word [si]
                mov word [boot_table], ax
                mov ax, word [si + 0x02]
                mov word [boot_table + 0x02], ax 

                mov si, __message
                call print

                mov ax, 0x4F02
                mov bx, 0x4000 | 0x115
                int 0x10

                mov ax, 0x4F01
                mov cx, 0x115
                mov di, vesa_mode
                int 0x10

.go_to_pm:
                cli
                lgdt [__gdt_desc]

                mov eax, cr0
                or eax, 1
                mov cr0, eax

                jmp __gdt.code:pm

global vesa_mode
vesa_mode:      resb 0x200
__out_of_mode_msg: db "Out Of Mode", 0xd, 0xa, 0x0
__gdt:
.null:  equ $ - __gdt
        dq 0x00000000
.code:  equ $ - __gdt
        dw 0xFFFF
        dw 0x0000
        db 0x00
        db 0x9A
        db 0xCF
        db 0
.data:  equ $ - __gdt
        dw 0xFFFF
        dw 0x0000
        db 0x00
        db 0x92
        db 0xCF
        db 0
__gdt_desc: dw $ - __gdt - 1
            dd __gdt

__idt16:        dw 0x3FF
                dd 0

[bits 32]
section .text
pm:             mov ax, 0x10
                mov ds, ax
                mov es, ax
                mov fs, ax
                mov gs, ax
                mov ss, ax

                [extern load_stage]
                call load_stage

                hlt
                jmp $

global inb
global inw
global outb
global outw
inb:    xor eax, eax
        mov dx, word [esp + 4]
        in al, dx
        ret
inw:    xor eax, eax
        mov dx, word [esp + 4]
        in  ax, dx
        ret
outb:   mov ax, word [esp + 8]
        mov dx, word [esp + 4]
        out dx, al
        ret
outw:   mov ax, word [esp + 8]
        mov dx, word [esp + 4]
        out dx, ax
        ret

__message: db "Loading Stage", 0
