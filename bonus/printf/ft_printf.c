#include "ft_printf.h"

int terminal_column = 0;
int terminal_row = 0;
uint16_t* terminal_buffer = (uint16_t*)0xB8000;

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

void flush_screen() {
    for (int i = 0; i < 80 * 25; i++) {
        terminal_buffer[i] = (uint16_t)' ' | (uint16_t)0x0F << 8;
    }
    terminal_column = 0;
    terminal_row = 0;
    update_hardware_cursor(0, 0);
}

void terminal_putchar(char c, color_t color) {
    if (c == '\b') {
        if (terminal_column > 0) {
            terminal_column--;
        } else if (terminal_row > 0) {
            terminal_row--;
            terminal_column = 79;
        }
        terminal_buffer[terminal_row * 80 + terminal_column] = (uint16_t)' ' | (uint16_t)0x0B << 8;
        update_hardware_cursor(terminal_column, terminal_row);
        return;
    }
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

int	ft_case(char c, va_list res, int len, color_t color)
{
	if (c == 'c')
		len += ft_putchar(va_arg(res, int), color);
	else if (c == 's')
		len += ft_putstr(va_arg(res, char *), color);
	else if (c == 'p')
		len = ft_putnbr_base_exa(va_arg(res, unsigned long long), len, color);
	else if (c == 'd')
		len = ft_putnbr(va_arg(res, int), len, color);
	else if (c == 'i')
		len = ft_putnbr(va_arg(res, int), len, color);
	else if (c == 'u')
		len = ft_putnbr_unsigned(va_arg(res, int), len, color);
	else if (c == 'x')
		len = ft_putnbr_exa(va_arg(res, unsigned long long), EXA_DOWN, len, color);
	else if (c == 'X')
		len = ft_putnbr_exa(va_arg(res, int), EXA_UP, len, color);
	else if (c == '%')
		len += ft_putchar('%', color);
	return (len);
}

int	ft_printf(color_t color, const char *str, ...)
{
	va_list	lst;
	int		i;
	int		ibis;

	i = 0;
	ibis = 0;
	va_start(lst, str);
	while (str[i])
	{
		if (str[i] == '%')
		{
			ibis = ft_case(str[i + 1], lst, ibis, color);
			i++;
		}
		else
		{
			ft_putchar(str[i], color);
			ibis++;
		}
		i++;
	}
	va_end(lst);
	return (ibis);
}