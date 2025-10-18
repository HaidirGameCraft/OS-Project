section .text
global page_reload_cr3
page_reload_cr3:
    mov eax, [esp + 4]
    mov cr3, eax
    ret

global invlpage
invlpage:
    mov eax, [esp + 4]
    invlpg [eax]
    ret