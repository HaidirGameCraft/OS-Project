
%define FAT_HEADER                  0x7c00 + 3

; READ DISK - read each sector from disk
; bx - The Destination Address
; ax - LBA Address
; cx - read sector counter
; dl - drive number
__read_disk:    
                    push ax
                    push cx
                    push bx
                    push dx
.__lba_converter:   
                    xor dx, dx
                    div word [FAT_HEADER + 21]

                    inc dx
                    mov byte [sector], dl

                    xor dx, dx
                    div word [FAT_HEADER + 23]
                    mov byte [head], dl
                    mov byte [cylinder], al
                    
                    mov di, 0
                    pop dx
                    pop bx
.__try_again:       ; end LBA Converter
                    mov ah, 0x02
                    mov al, 0x01
                    mov ch, byte [cylinder]
                    mov cl, byte [sector]
                    mov dh, byte [head]
                    int 0x13
                    jnc .__end_read_disk
                    
                    mov ah, 0x00
                    mov al, 0x00
                    int 0x13
                    jc .__error_read_disk

                    inc di
                    cmp di, 5
                    jl .__try_again
.__error_read_disk:
                    pop cx
                    pop ax
                    mov si, error_message
                    call print

                    hlt
                    jmp $

.__end_read_disk:    
                    pop cx
                    pop ax

                    inc ax
                    add bx, word [FAT_HEADER + 8]

                    dec cx
                    cmp cx, 0
                    jg __read_disk

                    mov si, complete_message
                    call print

                    ret

; __ext_read_sector: the read sector by using LBA Address
; - ax: The LBA Address
; - bx: Buffer pointer
; - cx: size of sector to read
; - dl: disk drive number
__ext_read_sector:  push ax
                    push ebx

                    mov word [.dap + 0x08], ax
                    mov dword [.dap + 0x04], ebx

                    mov di, 0
.try_again:         mov ah, 0x42
                    mov al, 0x00
                    mov si, .dap
                    int 0x13
                    jnc .done

                    mov ax, 0
                    int 0x13
                    jc .error_disk

                    inc di
                    cmp di, 5
                    jl .try_again
.error_disk:        mov si, error_message
                    call print

                    hlt
                    jmp $
.done:              pop ebx
                    pop ax

                    inc ax
                    add ebx, 512
                    dec cx
                    test cx, cx
                    jnz __ext_read_sector

                    ret
.dap:               db 0x10
                    db 0x00
                    dw 0x01
                    dd 0x00000000   ;   the buffer pointer address
                    dd 0x00000000   ;   low LBA Address
                    dd 0x00000000   ;   higher LBA Address
error_message:     db "Error: Cannot read disk", 0xa, 0xd, 0
complete_message:  db "Read complete", 0xa, 0xd, 0 
sector: dw 0
head: dw 0
cylinder: dw 0
