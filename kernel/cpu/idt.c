#include <cpu/idt.h>
#include <cpu/isr.h>
#include <stdio.h>
#include <cpu/port.h>
#include <timer.h>
#include <driver/keyboard.h>

idt_entry_t idt_entries[256];

idt_desc idt_ptr;
u32 interrupt_function[256];

const char* interrupt_message[] = {
    "Divide Error",
    "Debug Exception",
    "NMI Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "No Math Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection",
    "Page Fault",
    "Reserved",
    "Floating-Point Error",
    "Alignment Check",
    "Machine Check",
    "Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

extern void __isr0();
extern void __isr1();
extern void __isr31();

extern void __irq0();
extern void __irq1();
extern void __irq15();  

extern void* isr_stub_table[];
extern void* irq_stub_table[];
extern void syscall_handle();

void remapped_PIC();

void idt_initialize() {
    __asm__ volatile("cli");
    for(int i = 0; i < 32; i++) {
        //printf("ISR Address: %x\n", (u32) i );
        idt_setentry(i, ((u32*) isr_stub_table)[i], 0x08, 0x8E);
    }

    // remapped PIC before entering IRQ interrupt
    remapped_PIC();

    // Setting up IRQ handle
    for(int i = 0; i < 16; i++) {
        idt_setentry(i + 32, ((u32*) irq_stub_table)[i], 0x08, 0x8E);
    }

    // Setting up Syscall Interrupt
    idt_setentry(0x80, (u32) syscall_handle, 0x08, 0x8E);

    idt_ptr.limit = sizeof( idt_entry_t ) * 256 - 1;
    idt_ptr.base = (u32) &idt_entries;

    idt_install( &idt_ptr );

    timer_interrupt_setup();
    Keyboard_Initialize();

    __asm__ volatile("sti");
}

void idt_setentry(  int number,
                    u32 offset,
                    u16 segment, 
                    u8 attribute) {
    idt_entry_t* entry = &idt_entries[number];

    //printf("Address Int Attach: 0x%x\n", offset);
    entry->low_offset = offset & 0xFFFF;
    entry->high_offset = (offset >> 16) & 0xFFFF;

    entry->segment = segment;
    entry->reserved = 0;
    entry->attributes = attribute;
}

void isr_handle( register_t reg ) {
    if( reg.number_interrupt >= 32 )
        return;

    printf("Error Code:\n");
    printf("EAX = 0x%x\nEBX = 0x%x\nECX = 0x%x\nEDX = 0x%x\nESP = 0x%x\n", reg.eax, reg.ebx, reg.ecx, reg.edx, reg.esp);
    printf("EBP = 0x%x\nEDI = 0x%x\nESI = 0x%x\n", reg.ebp, reg.edi, reg.esi);
    printf("Int Number = 0x%x\n Error Code = 0x%x\n", reg.number_interrupt, reg.error_code);
    printf("Message Interrupt: ");
    printf(interrupt_message[reg.number_interrupt]);
    printf("\n");

    if( reg.number_interrupt == 14) {
        u32 fault_addr;
        asm volatile("mov %%cr2, %0" : "=r"(fault_addr));

        printf("[PAGE FAULT] at 0x%x (err=%x)\n", fault_addr, reg.error_code);

        

        // Decode error code bits
        if (reg.error_code & 1) printf("  -> Page present\n");
        else printf("  -> Page not present\n");
        if (reg.error_code & 2) printf("  -> Write caused fault\n");
        else printf("  -> Read caused fault\n");
        if (reg.error_code & 4) printf("  -> In user mode\n");
        else printf("  -> In kernel mode\n");
        if (reg.error_code & 8) printf("  -> Reserved bit violation\n");
        if (reg.error_code & 16) printf("  -> Caused by instruction fetch\n");     
    }
    else {

    }

    

   for(;;) { __asm__ volatile("cli; hlt"); };
}

void irq_handle( register_t reg ) {
    if( reg.number_interrupt >= 0x40 )
        outb(0xA0, 0x20);
    outb(0x20, 0x20);

    int interrupt_nmbr = reg.number_interrupt;

    if( interrupt_function[interrupt_nmbr] != 0 )
    {
        void (*__int)( register_t ) = (void (*)( register_t )) interrupt_function[interrupt_nmbr];
        __int( reg );
    }
}

void set_interrupt_function(u32 address, int index) {
    interrupt_function[index] = address;
}

void remapped_PIC() {
    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    outb(0x21, 0x20);
    outb(0xA1, 0x28);

    outb(0x21, 0x04);
    outb(0xA1, 0x02);

    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    outb(0x21, 0x00);
    outb(0xA1, 0x00);
}
