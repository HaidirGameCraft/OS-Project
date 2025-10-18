; PRINT - printing the text/message in BIOS Mode
; si - Message Address
print:          xor ax, ax
                mov ah, 0x0E

.print_loop:    mov al, byte [si]
                inc si
                cmp al, 0
                je .done

                int 0x10
                jmp .print_loop
.done:          ret
