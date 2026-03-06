#include <stdint.h>

typedef uint8_t color_t;
uint16_t* terminal_buffer = (uint16_t*)0xB8000;
int terminal_column = 0;
int terminal_row = 0;

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ __volatile__ ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

void update_hardware_cursor(int x, int y) {
    uint16_t pos = y * 80 + x;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t) (pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

void scroll_screen() {
    for (int i = 0; i < 80 * 24; i++) {
        terminal_buffer[i] = terminal_buffer[i + 80];
    }
    for (int i = 80 * 24; i < 80 * 25; i++) {
        terminal_buffer[i] = (uint16_t)' ' | (uint16_t)0x0F << 8;
    }
}

void terminal_putchar(char c, color_t color) {
    if (c == '\n') {
        terminal_column = 0;
        terminal_row++;
    } else {
        terminal_buffer[terminal_row * 80 + terminal_column] = (uint16_t)c | (uint16_t)color << 8;
        terminal_column++;
        if (terminal_column >= 80) {
            terminal_column = 0;
            terminal_row++;
        }
    }
    if (terminal_row >= 25) {
        scroll_screen();
        terminal_row = 24;
    }
    update_hardware_cursor(terminal_column, terminal_row);
}

void terminal_write_string(const char* data, color_t color) {
    for (int i = 0; data[i] != '\0'; i++) {
        terminal_putchar(data[i], color);
    }  
}

void flush_screen() {
    for (int i = 0; i < 80 * 25; i++) {
        terminal_buffer[i] = (uint16_t)' ' | (uint16_t)0x0F << 8;
    }
    update_hardware_cursor(0, 0);
}

// void kernel_main() {

//     flush_screen();
//     terminal_write_string("Welcome to KFS!\n", 0x0A);
//     terminal_write_string("42", 0x0A);
// }

void kernel_main() {
    flush_screen();

    // On affiche 30 lignes pour forcer le scroll (l'écran fait 25 lignes)
    for (int i = 0; i < 30; i++) {
        // On change de couleur pour bien voir le défilement
        // i % 15 + 1 permet de varier les couleurs de 1 à 15
        color_t color = (i % 15) + 1; 
        
        terminal_write_string("Ceci est la ligne numero: ", color);
        
        // Petit trick pour afficher le nombre (vu qu'on n'a pas encore printf)
        if (i >= 10) {
            terminal_putchar((i / 10) + '0', color);
        }
        terminal_putchar((i % 10) + '0', color);
        
        terminal_putchar('\n', color);
    }

    terminal_write_string("Si tu vois ce message, le scroll a fonctionne !", 0x0E); // Jaune
}