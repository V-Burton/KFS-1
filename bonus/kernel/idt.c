#include <stdint.h>
#include "idt.h"
#include "gdt.h"

idt_entry_t idt[256];
idt_ptr_t idt_ptr;
keyboard_state_t kbd_state = {0};
static char input_buffer[256];
static int input_len = 0;

// Table de correspondance pour le clavier AZERTY (Scancode Set 1)
unsigned char kbd_map_lower[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', ')', '=', '\b',
    '\t', 'a', 'z', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '^', '$', '\n',
    0, 'q', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'm', '0', '`',
    0, '*', 'w', 'x', 'c', 'v', 'b', 'n', ',', ';', ':', '!', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

unsigned char kbd_map_upper[128] = {
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'A', 'Z', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'Q', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'M', '0', '`',
    0, '|', 'W', 'X', 'C', 'V', 'B', 'N', ',', ';', ':', '"', 0,
    '|', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};


// Envoie un octet sur un port spécifique
static inline void outb(uint16_t port, uint8_t data) {
    __asm__ volatile("outb %0, %1" : : "a"(data), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Petite pause pour laisser le temps au PIC de traiter les commandes
static inline void io_wait() {
    __asm__ volatile("outb %%al, $0x80" : : "a"(0));
}

void pic_remap() {
    // ICW1 : Début de l'initialisation
    outb(0x20, 0x11);
    io_wait();
    outb(0xA0, 0x11);
    io_wait();

    //ICW2 : Les OFFSETS
    outb(0x21, 0x20); // 0x20 = 32 en decimal (master PIC)
    io_wait();
    outb(0xA1, 0x28); // 0x28 = 40 en décimal (Slave PIC)
    io_wait();

    // ICW3 : Configuration de la cascade (Comment les PIC communiquent)
    outb(0x21, 0x04);
    io_wait();
    outb(0xA1, 0x02);
    io_wait();

    // ICW4 : Mode 8086
    outb(0x21, 0x01);
    io_wait();
    outb(0xA1, 0x01);
    io_wait();

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
    for (int i = 0; i < 256; i++) {
        // set_idt_gate(i, (uint32_t)isr0);
        set_idt_gate(i, (uint32_t)isr_default);
    }

    set_idt_gate(33, (uint32_t)irq1);

    set_idt_gate(8,  (uint32_t)isr8);  // Double Fault
    set_idt_gate(13, (uint32_t)isr13); // General Protection Fault
    set_idt_gate(14, (uint32_t)isr14);

    pic_remap(); // Remapper le PIC pour éviter les conflits avec les exceptions CPU


    __asm__ volatile("lidt (%0)" : : "r"(&idt_ptr));  // Charger l'IDT
    __asm__ volatile("sti"); // Active les interruptions
}

void print_cpu_info() {
    uint32_t eax, ebx, ecx, edx;
    char vendor[13];

    __asm__ volatile("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(0));
    ((uint32_t*)vendor)[0] = ebx;
    ((uint32_t*)vendor)[1] = edx;
    ((uint32_t*)vendor)[2] = ecx;
    vendor[12] = '\0';
    
    terminal_write_string("CPU Vendor: ", 0x0B);
    terminal_write_string(vendor, 0x0F);
    terminal_putchar('\n', 0x0F);
}

static int str_eq(const char *a, const char *b) { 
    while (*a && *b) {
        if (*a++ != *b++)
            return 0;
    }
    return *a == *b;
}

void evaluate_command(const char *cmd) { 
    if (str_eq(cmd, "proc")) {
        print_cpu_info();
    } else if (str_eq(cmd, "clear")) {
        flush_screen();
    } else if (str_eq(cmd, "help")) {
        terminal_write_string("Commandes disponibles :\n", 0x07);
        terminal_write_string("  proc  - Affiche les informations CPU\n", 0x07);
        terminal_write_string("  clear - Efface l'écran\n", 0x07);
        terminal_write_string("  help  - Affiche cette aide\n", 0x07);
    }
}

void handle_keyboard(uint8_t scancode) {
    if (scancode == 0x2A || scancode == 0x36) { // Shift Gauche enfoncé
        kbd_state.shift = 1;
        return;
    }
    if (scancode == 0xAA || scancode == 0xB6) { // Shift Gauche relâché (0x2A + 0x80)
        kbd_state.shift = 0;
        return;
    }

    if (scancode)

    if (scancode < 128) {
        char key = (kbd_state.shift) ? kbd_map_upper[scancode] : kbd_map_lower[scancode];
        if (key == '\b') {
            if (input_len > 0) {
                input_len++;
                terminal_putchar('\b', 0x07);
            }
        } else if (key == '\n') {
            terminal_putchar('\n', 0x07);
            input_buffer[input_len] = '\0';
            evaluate_command(input_buffer);
            input_len = 0;
        } else if (key != 0 && input_len < 255) {
            input_buffer[input_len++] = key;
            terminal_putchar(key, 0x07);
        }
    }
}

void interrupt_handler(struct registers *r) {

    if (r->int_no == 33) { // IRQ1 - clavier
        uint8_t scancode = inb(0x60); // Lecture de l´octet sur le port du clavier}
        handle_keyboard(scancode);
    }
    // Envoyer e signal End of Interrup (EOI) au PIC sinon l'interrupt ne sera jamais considerer comme finis et donc bloquage..
    if (r->int_no >= 32) {
        if (r->int_no >= 40) outb(0xA0, 0x20); // Esclave
        outb(0x20, 0x20); // Maître
    }
}

void keyboard_handler(void) {
    uint8_t scancode = inb(0x60);
    handle_keyboard(scancode);
    // EOI au PIC master
    outb(0x20, 0x20);
}