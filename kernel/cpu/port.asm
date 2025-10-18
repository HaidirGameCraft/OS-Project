section .text
global inb
inb:    mov edx, dword [esp + 4]
        xor eax, eax
        in al, dx
        ret

global outb
outb:   mov edx, dword [esp + 4]
        mov eax, dword [esp + 8]
        out dx, al
        ret

global inw
inw:    mov edx, dword [esp + 4]
        xor eax, eax
        in ax, dx
        ret

global outw
outw:   mov edx, dword [esp + 4]
        mov eax, dword [esp + 8]
        out dx, ax
        ret
        
