[bits 16]
[org 0x0600]

; MBR BootCode
; Based on OSDev (MBR x86): [https://wiki.osdev.org/MBR_(x86)]
; - The boot code is loaded on 0x7C00 as current boot code. So, need to start at 0x0600 address by copy the hold code 
;   of MBR and replace it on 0x0600
; - finding the active partition
; - copy the boot code from active partiton and replace on 0x7C00 address
; - after replace jump to 0x7C00


__start:
                cli
                mov ax, 0x00
                mov es, ax
                mov ds, ax
                mov fs, ax
                mov gs, ax
                mov ss, ax

                mov byte [DriveNumber], dl

._copyMBR:
                mov cx, 256     ; - 256 words
                mov si, 0x7C00  ; - The address source of code
                mov di, 0x0600  ; - The address destination of code to copy
.loop:
                mov ax, word [si]
                mov word [di], ax
                add si, 2
                add di, 2

                dec cx
                cmp cx, 0
                jg .loop
                ; After copy the MBR Code we can jump from 0x7C00 to 0x0600
                jmp 0x00:__LowMemStart

; MBR Based Code start
; - On Partition Table check the attributes where 7 bits set 1 ( active partition )
; - if the active part set to 1, mean that part will need to copy to 0x7C00 address
__LowMemStart:              sti

                            xor cx, cx
                            cmp cx, 4
                            je .__NotFoundActivePart
                            mov si, part1
.__StartFindActivePart:     mov al, byte [si]
                            test al, 1 << 7
                            jz .__FindOtherActivePart

                            ; When Found
                            mov word [PartOffset], si       ; save the partition offset
                            add si, 0x08
                            mov ax, word [si]
                            mov bx, 0x7C00
                            mov dl, byte [DriveNumber]
                            mov cx, 0x01
                            call read_sector

                            mov si, word [PartOffset]
                            mov dl, byte [DriveNumber]
                            jmp 0x00:0x7C00

                            hlt
                            jmp $

.__FindOtherActivePart:     add si, 16
                            jmp .__StartFindActivePart
.__DiskError:               mov si, DiskError_Message
                            call print
                            
                            hlt
                            jmp $
.__NotFoundActivePart:      
                            mov si, NOTFOUNDACTIVEPART_MESSAGE
                            call print

                            hlt
                            jmp $
; Print the message to the display on BIOS
; - si -> the text address
print:
                mov ah, 0x0E
.main:          mov al, byte [si]
                inc si
                test al, al
                jz .done

                int 0x10
                jmp .main
.done:          ret

; read_sector: read the sector by using LBA Address
; ax - 16 bits lower LBA Address
; cx - counter read
; bx - the destination address
; dl - disk number
read_sector:        push ax
                    push bx
                    mov word [.dap + 0x08], ax
                    mov word [.dap + 0x04], bx
                    
                    mov di, 0
.try_again:
                    mov ah, 0x42
                    mov al, 0
                    mov si, .dap
                    int 0x13
                    jnc .done

                    mov ah, 0
                    mov al, 0
                    int 0x13
                    jc .error_read_disk
                    
                    inc di
                    cmp di, 5
                    jl .try_again
.error_read_disk:   mov si, ReadDiskError_Message
                    call print

                    hlt
                    jmp $
.done:
                    pop bx
                    pop ax

                    inc ax
                    add bx, 512
                    dec cx
                    test cx, cx
                    jl read_sector

                    ret

.dap:           db 0x10
                db 0x00
                dw 0x01
                dd 0x00000000
                dw 0x0000       ; - 16 bits lower-lower LBA Address
                dw 0x0000       ; - 16 bits lower-higher LBA Address
                dd 0x00000000   ; - 32 bits Higher LBA Address

PartOffset: dw 0x0000
DriveNumber: db 0x80
NOTFOUNDACTIVEPART_MESSAGE: db "The Active Partition (MBR) is Not Found", 0xa, 0xd, 0
DiskError_Message: db "Disk Error (MBR) Cannot read", 0xa, 0xd, 0
ReadDiskError_Message: db "Read Sector from Disk Failed", 0xa, 0xd, 0
Sector:     db 0x00
Cylinder:   db 0x00
Heads:      db 0x00

times 440-($-$$) db 0
disk_signature: dd 0x00000000
disk_reserved: dw 0x0000

; The Partition Tables
part1: times 16 db 0
part2: times 16 db 0
part3: times 16 db 0
part4: times 16 db 0
dw 0xAA55