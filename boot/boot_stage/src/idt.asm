section .text
global idt_install
idt_install:    mov eax, [esp + 4]
                lidt [eax]
                ret

%macro PUSHALL 0 
    push eax
    push ecx
    push edx
    push ebx
    push esp
    push ebp
    push esi
    push edi
%endmacro
%macro POPALL 0
    pop edi
    pop esi
    pop ebp
    pop esp
    pop ebx
    pop edx
    pop ecx
    pop eax
%endmacro
%macro isr_stub 1
global __isr%1:
__isr%1:    cli
            push byte %1
            jmp isr_handle_stub
%endmacro

%macro irq_stub 2
global __irq%2:
__irq%2:    cli
            push byte %2
            push byte %1
            jmp irq_handle_stub
%endmacro

isr_handle_stub:    PUSHALL
                    [extern isr_handle]
                    call isr_handle
                    POPALL
                    add esp, 8
                    sti
                    iret

isr_stub 0
isr_stub 1
isr_stub 2
isr_stub 3
isr_stub 4
isr_stub 5
isr_stub 6
isr_stub 7
isr_stub 8
isr_stub 9
isr_stub 10
isr_stub 11
isr_stub 12
isr_stub 13
isr_stub 14
isr_stub 15
isr_stub 16
isr_stub 17
isr_stub 18
isr_stub 19
isr_stub 20
isr_stub 21
isr_stub 22
isr_stub 23
isr_stub 24
isr_stub 25
isr_stub 26
isr_stub 27
isr_stub 28
isr_stub 29
isr_stub 30
isr_stub 31

global isr_stub_table
isr_stub_table:
%assign i 0
%rep 32
    dd __isr%+i
%assign i i+1
%endrep