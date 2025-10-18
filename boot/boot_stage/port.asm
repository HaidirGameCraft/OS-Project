section .text
global iportb
iportb:     xor eax, eax
            mov dx, word [esp + 4]
            in  al, dx
            ret

global iportw
iportw:     xor eax, eax
            mov dx, word [esp + 4]
            in  ax, dx
            ret

global oportb
oportb:     mov ax, word [esp + 8]
            mov dx, word [esp + 4]
            out dx, al
            ret

global oportw
oportw:     mov ax, word [esp + 8]
            mov dx, word [esp + 4]
            out dx, ax
            ret