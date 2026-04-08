#include <stdint.h>

uint16_t* terminal_buffer = (uint16_t*)0xB8000;
int terminal_column = 0;
int terminal_row = 0;

void terminal_putchar(char c) {
    if (c == '\n') {
        terminal_column = 0;
        terminal_row++;
        return;
    }
    terminal_buffer[terminal_row * 80 + terminal_column] = (uint16_t)c | (uint16_t)0x0F << 8;
    terminal_column++;
    if (terminal_column >= 80) {
        terminal_column = 0;
        terminal_row++;
    }
    if (terminal_row >= 25) {
        terminal_row = 0;
    }
}

void terminal_write_string(const char* data) {
    for (int i = 0; data[i] != '\0'; i++) {
        terminal_putchar(data[i]);
    }  
}

void flush_screen() {
    for (int i = 0; i < 80 * 25; i++) {
        // Chaque caractère occupe 16 bits:
        // 0-7 pour le caractère et 8-15 pour l'attribut (couleur fond et texte).
        terminal_buffer[i] = (uint16_t)' ' | (uint16_t)0x0F << 8;
    }
}

void kernel_main() {

    flush_screen();
    terminal_write_string("42");
}