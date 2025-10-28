section .text

global read 
read:   mov eax, 0x01
        mov edx, dword [esp + 4]        ; FILE* file
        mov ebx, dword [esp + 8]        ; char* Buffer
        mov ecx, dword [esp + 12]       ; size_t Size
        int 0x80
        ret

global write
write:      mov eax, 0x02
            ret

global open
open:   mov eax, 0x03                   
        mov ebx, dword [esp + 4]        ; char* filename
        int 0x80
        ret

global close
close:      mov eax, 0x04
            mov edx, dword [esp + 4]    ; FILE* file
            int 0x80
            ret
            
global page_map
page_map:   mov eax, 0x05
            mov ebx, dword [esp + 4]    ; addr_t Virtual_address
            mov edx, dword [esp + 8]    ; u8 Flags
            mov ecx, dword [esp + 12]   ; size_t Size
            int 0x80
            ret

global page_vmap
page_vmap:      mov eax, 0x06
                mov ebx, dword [esp + 4]    ; addr_t VirtualAddr
                mov edi, dword [esp + 8]    ; addr_t PhysicalAddr
                mov edx, dword [esp + 12]   ; u8 Flags
                mov ecx, dword [esp + 16]   ; size_t Size
                int 0x80
                ret

global page_unmap
page_unmap:     mov eax, 0x07
                mov ebx, dword [esp + 4]    ; addr_t VirtualAddr
                mov ecx, dword [esp + 8]    ; size_t Size
                int 0x80
                ret