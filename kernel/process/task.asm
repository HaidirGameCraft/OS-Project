section .text

global TaskInitialize
TaskInitialize:     mov edi, dword [esp + 4]                    ; get first Argument pointer
                    mov eax, cr3
                    mov dword [edi + 4], eax
                    ret

global __switch_task__
; esp + 4: Current Thread
; esp + 8: Next Thread
__switch_task__:    pusha
                    mov eax, cr3
                    push eax
                    pushf

                    mov edi, dword [esp + 44]
                    mov dword [edi + 12], esp

                    mov eax, dword [esp]
                    mov dword [edi + 8], eax                    ; Save EFLAGS

                    mov eax, dword [esp + 4]
                    mov dword [edi + 4], eax                    ; save CR3

                    mov eax, dword [esp + 40]
                    mov dword [edi], eax                        ; save EIP

                    ; Reload next Task
                    mov edi, dword [esp + 48]
                    mov esp, dword [edi + 12]                   ; restore ESP
                    popf
                    pop eax
                    mov cr3, eax
                    popa
                    ret

global __switch_task_single__
__switch_task_single__: mov edi, dword [esp + 4]
                        mov esp, dword [edi + 12]
                        
                        popf
                        pop eax
                        mov cr3, eax
                        popa
                        ret


