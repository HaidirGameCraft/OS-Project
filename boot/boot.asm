[org 0x7c00]
[bits 16]

jmp short __start
nop
; Fat 32 Configuration
fat_header: ; + 3
.oem_identifier:        db "MSWIN4.1"       ; - 0
.bytes_per_sectors:     dw 512              ; - 8
.sectors_per_cluster:   db 1                ; - 10
.reserved_sectors:      dw 0                ; - 11
.fat_number:            db 0x02             ; - 13
.root_dir_entries:      dw 0                ; - 14
.total_sectors:         dw 0                ; - 16
.media_type:            db 0                ; - 17
.sectors_per_fat:       dw 0                ; - 19
.sectors_per_track:     dw 0                ; - 21
.heads:                 dw 0                ; - 23
.hidden_sectors:        dd 0                ; - 25
.large_sector_count:    dd 0                ; - 29

%ifdef USE_FAT32_FILESYSTEM
fat_32_extended_boot_record:
.sectors_per_fat:       dd 0                ; - 33
.flags:                 dw 0                ; - 35
.fat_version:           dw 0                ; - 37
.cluster_number:        dd 0                ; - 39
.sector_of_FSInfo:      dw 0                ; - 43
.sector_of_backup:      dw 0                ; - 45
.reserved:              times 12 db 0       ; - 47
%endif

fat_boot_record:
.drive_number:          db 0x80             ; - 59
.flags_window_nt:       db 0                ; - 60
.signature:             db 0x29             ; - 61
.volume_ID:             dd 0                ; - 62
.volume_Label:          db "LOCAL DISK", 0  ; - 66
.system_identifier:     db "FAT32   "       ; - 77

%ifdef USE_FAT32_FILESYSTEM
times 90-($ - fat_header) db 0
%else
times 62-($ - fat_header) db 0
%endif

; MBR Boot Start
; ax - Partition Offset
; dl - Drive Number
__start:    mov sp, 0x7c00

            mov byte [fat_boot_record.drive_number], dl
            mov word [boot_table], si

            mov ax, word [si + 0x08]
            mov word [boot_table + 0x02], ax

            mov si, test_msg
            call print

            mov bx, 0
            mov es, bx

            ; mov dl, [fat_boot_record.drive_number]
            ; mov ax, 3
            ; mov bx, 0x8000
            ; mov cx, 10
            ; call __read_disk

            mov dl, byte [fat_boot_record.drive_number]
            mov ax, 3
            add ax, word [boot_table + 0x02]

            mov bx, 0x8000
            mov cx, 10
            call __ext_read_sector

            mov si, load_stage
            call print

            mov si, boot_table  ; set si to boot_table address
            push si
            jmp 0x8000

            hlt
            jmp $

__loadstage_address equ 0x8000

%include "boot/print.asm"
%include "boot/disk.asm"

test_msg: db "Hello, World", 0xa, 0xd, 0
load_stage: db "Go to Load Stage", 0xa, 0xd, 0

boot_table:     dw 0x0000       ; Partition Offset
                dd 0            ; LBA Partition Start

times 510-($-$$) db 0
dw 0xAA55
            
            
