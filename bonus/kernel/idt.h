#ifndef IDT_H
#define IDT_H

#include "../printf/ft_printf.h"
#include "microshell.h"

typedef uint8_t color_t;

// Structure d'une entrée de l'IDT (8 octets)
struct idt_entry_struct {
    uint16_t base_lo; // Addresse basse de la fonction ISR
    uint16_t sel;      // Le selecteur de segment de code (GDT)
    uint8_t always0;   // Heritage historique et doit toujours être à 0
    uint8_t flags;     // Privilèges de la gate et type de gate (DPL, P, ...)
    uint16_t base_hi;  // Suite de l'addresse de la fonction ISR;
} __attribute__ ((packed));

struct idt_ptr_struct {
    uint16_t limit; // Taille de l'IDT - 1
    uint32_t base;  // Addresse du premier element de l'IDT
} __attribute__ ((packed));

struct registers {
    uint32_t edi, esi, ebp, esp_dummy, ebx, edx, ecx, eax; //poussée par pushad
    uint32_t int_no, err_code;                       // poussée mnuellement
    uint32_t eip, cs, eflags, useresp, ss;          // poussée par le cpu
};

typedef struct idt_entry_struct idt_entry_t;
typedef struct idt_ptr_struct idt_ptr_t;

void setup_idt();
void interrupt_handler(struct registers *r);
extern void irq1();
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr_reserved();
extern void isr_default();

#endif // IDT_H