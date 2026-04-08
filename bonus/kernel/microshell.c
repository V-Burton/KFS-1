#include <stdint.h>
#include "microshell.h"
#include "gdt.h"

static char input_buffer[256];
static int input_len = 0;
keyboard_state_t kbd_state = {0};

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

void print_cpu_info() {
    uint32_t eax, ebx, ecx, edx;
    char vendor[13];

    __asm__ volatile("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(0));
    ((uint32_t*)vendor)[0] = ebx;
    ((uint32_t*)vendor)[1] = edx;
    ((uint32_t*)vendor)[2] = ecx;
    vendor[12] = '\0';
    
    ft_printf(0x0B, "CPU Vendor: ");
    ft_printf(0x0F, "%s\n", vendor);
}

void dump_stack(int line) {
    uint32_t *esp;
    __asm__ volatile("mov %%esp, %0" : "=r"(esp));
    uint32_t *ptr = (uint32_t*)esp;
    
    ft_printf(0x0C, "--- Stack dump (ESP: %X) ---\n", esp);
    for (int i = 0; i < line; i++) {
        uint32_t val = *(ptr + i);
        ft_printf(0x0F, "[ESP+%02d] 0x%X : 0x%08X | ", i * 4, ptr + i, val);
        for (int j = 0; j < 4; j++) {
            char c = (val >> (j * 8)) & 0xFF;
            if (c >= 32 && c <= 126) {
                ft_printf(0x0F, "%c", c);
            } else {
                ft_printf(0x08, ".");
            }
        }
        ft_printf(0x0F, "\n");
    }
}

static int str_eq(const char *a, const char *b) { 
    while (*a && *b) {
        if (*a++ != *b++)
            return 0;
    }
    return *a == *b;
}

static int str_eq_n(const char *a, const char *b, int n) { 
    while (*a && *b && n > 0) {
        if (*a++ != *b++)
            return 0;
        n--;
    }
    return *a == *b;
}

void evaluate_command(const char *cmd) { 
    if (str_eq(cmd, "proc")) {
        print_cpu_info();
    } else if (str_eq(cmd, "clear")) {
        flush_screen();
    } else if (str_eq(cmd, "help")) {
        ft_printf(0x0A, "Commandes disponibles :\n");
        ft_printf(0x0A, "  proc  - Affiche les informations CPU\n");
        ft_printf(0x0A, "  clear - Efface l'ecran\n");
        ft_printf(0x0A, "  addr_gdt - Affiche l'adresse de la GDT\n");
        ft_printf(0x0A, "  help  - Affiche cette aide\n");
        ft_printf(0x0A, "  stack_dump <n> - Affiche les n premieres ligne de la stack. (max 24)\n");
    } else if (str_eq(cmd, "addr_gdt")) {
        gdt_ptr_t gdt_read;
        __asm__ __volatile__("sgdt %0" : "=m" (gdt_read));
        
        ft_printf(0x0A, "GDT Base Address: ");
        ft_printf(0x0B, "0x%x\n", gdt_read.base);
    } else if (str_eq_n(cmd, "stack_dump", 9)) {
        int n = 0;
        if (ft_atoi(cmd) == 1 && n > 0 && n <= 24) {
            dump_stack(n);
        } else {
            ft_printf(0x0C, "Usage: stack dump <n> (n doit etre entre 1 et 24)\n");
        }
    } else {
        ft_printf(0x0C, "Commande inconnue: %s\n", cmd);
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
    if (scancode < 128) {
        char key = (kbd_state.shift) ? kbd_map_upper[scancode] : kbd_map_lower[scancode];
        if (key == '\b') {
            if (input_len > 0) {
                input_len--;
                terminal_putchar('\b', 0x0B);
            }
        } else if (key == '\n') {
            terminal_putchar('\n', 0x0B);
            input_buffer[input_len] = '\0';
            evaluate_command(input_buffer);
            input_len = 0;
            ft_printf(0x0E, "KFS>");
        } else if (key != 0 && input_len < 255) {
            input_buffer[input_len++] = key;
            terminal_putchar(key, 0x0B);
        }
    }
}