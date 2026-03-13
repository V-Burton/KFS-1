#ifndef MAIN_H
#define MAIN_H

typedef uint8_t color_t;

void terminal_write_string(const char* data, color_t color);
void terminal_putchar(char c, color_t color);

#endif // MAIN_H