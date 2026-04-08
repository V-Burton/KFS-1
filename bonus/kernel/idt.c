#include <stdint.h>
#include "idt.h"
#include "gdt.h"

idt_entry_t idt[256];
idt_ptr_t idt_ptr;

// Envoie un octet sur un port spécifique
static inline void outb(uint16_t port, uint8_t data) {
    __asm__ volatile("outb %0, %1" : : "a"(data), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void pic_remap() {
    // ICW1 : Début de l'initialisation
    outb(0x20, 0x11);
    __asm__ volatile("outb %%al, $0x80" : : "a"(0)); // Petite pause pour laisser le temps au PIC de traiter les commandes
    outb(0xA0, 0x11);
    __asm__ volatile("outb %%al, $0x80" : : "a"(0));

    //ICW2 : Les OFFSETS
    outb(0x21, 0x20); // 0x20 = 32 en decimal (master PIC)
    __asm__ volatile("outb %%al, $0x80" : : "a"(0));
    outb(0xA1, 0x28); // 0x28 = 40 en décimal (Slave PIC)
    __asm__ volatile("outb %%al, $0x80" : : "a"(0));

    // ICW3 : Configuration de la cascade (Comment les PIC communiquent)
    outb(0x21, 0x04);
    __asm__ volatile("outb %%al, $0x80" : : "a"(0));
    outb(0xA1, 0x02);
    __asm__ volatile("outb %%al, $0x80" : : "a"(0));

    // ICW4 : Mode 8086
    outb(0x21, 0x01);
    __asm__ volatile("outb %%al, $0x80" : : "a"(0));
    outb(0xA1, 0x01);
    __asm__ volatile("outb %%al, $0x80" : : "a"(0));

    // Masquage: on désactive tout sauf le clavier pour
    outb(0x21, 0xFD); // Seul l'IRQ1 (clavier) est actif
    outb(0xA1, 0xFF);
}

void set_idt_gate(int n, uint32_t handler) {
    idt[n].base_lo = handler & 0xFFFF;
    idt[n].base_hi = (handler >> 16) & 0xFFFF;
    idt[n].sel = 0x08;
    idt[n].always0 = 0;
    idt[n].flags = 0x8E; //Present, ring 0 (kernel)
}

void setup_idt() {
    idt_ptr.limit = sizeof(idt_entry_t) * 256 - 1;
    idt_ptr.base = (uint32_t)&idt[0];

    // Initialisation de tous les descripteurs à une valeur par défaut
    set_idt_gate(0,  (uint32_t)isr0);   // Division par zéro
    set_idt_gate(1,  (uint32_t)isr1);   // Debug Exception
    set_idt_gate(2,  (uint32_t)isr2);   // Non Maskable Interrupt
    set_idt_gate(3,  (uint32_t)isr3);   // Breakpoint
    set_idt_gate(4,  (uint32_t)isr4);   // Overflow
    set_idt_gate(5,  (uint32_t)isr5);   // Bound Range Exceeded
    set_idt_gate(6,  (uint32_t)isr6);   // Invalid Opcode
    set_idt_gate(7,  (uint32_t)isr7);   // Device Not Available
    set_idt_gate(8,  (uint32_t)isr8);   // Double Fault
    set_idt_gate(9,  (uint32_t)isr9);   // Coprocessor Segment Overrun
    set_idt_gate(10, (uint32_t)isr10);  // Invalid TSS
    set_idt_gate(11, (uint32_t)isr11);  // Segment Not Present
    set_idt_gate(12, (uint32_t)isr12);  // Stack Segment Fault
    set_idt_gate(13, (uint32_t)isr13);  // General Protection Fault
    set_idt_gate(14, (uint32_t)isr14);  // Page Fault
    set_idt_gate(15, (uint32_t)isr15);  // Intel Reserved
    set_idt_gate(15, (uint32_t)isr16);  // x87 Floating-Point Exception
    set_idt_gate(16, (uint32_t)isr17);  // Alignment Check
    set_idt_gate(17, (uint32_t)isr18);  // Machine Check
    set_idt_gate(18, (uint32_t)isr19);  // SIMD Floating-Point Exception
    set_idt_gate(19, (uint32_t)isr20);  // Virtualization Exception
    set_idt_gate(20, (uint32_t)isr21);  // Control Protection Exception
    for (int i = 22; i < 32; i++) {
        set_idt_gate(i, (uint32_t)isr_reserved); // Interruption réservée
    }
    set_idt_gate(33, (uint32_t)irq1);
    for (int i = 34; i < 256; i++) {
        // set_idt_gate(i, (uint32_t)isr0);
        set_idt_gate(i, (uint32_t)isr_default);
    }
    pic_remap(); // Remapper le PIC pour éviter les conflits avec les exceptions CPU


    __asm__ volatile("lidt (%0)" : : "r"(&idt_ptr));  // Charger l'IDT
    __asm__ volatile("sti"); // Active les interruptions
}

void interrupt_handler(struct registers *r) {
    if (r->int_no < 32) {
        ft_printf(0x0C, "Exception: %d\n", r->int_no);
        return;
    } else if (r->int_no == 33) { // IRQ1 - clavier
        uint8_t scancode = inb(0x60); // Lecture de l´octet sur le port du clavier}
        handle_keyboard(scancode);
    }
    // Envoyer le signal End of Interrup (EOI) au PIC sinon l'interrupt ne sera jamais considerer comme finis et donc bloquage..
    if (r->int_no >= 32 && r->int_no <= 47) {
        if (r->int_no >= 40) 
            outb(0xA0, 0x20); // Esclave
        outb(0x20, 0x20); // Maître
    }
}