#ifndef FT_PRINTF_H
# define FT_PRINTF_H

# include <stdint.h>
# include <stddef.h>
# include <stdarg.h>

# define EXA_DOWN	"0123456789abcdef"
# define EXA_UP		"0123456789ABCDEF"
# define INT_MIN -2147483648
# define INT_MAX 2147483647

typedef uint8_t color_t;

void terminal_putchar(char c, color_t color);
void flush_screen(void);
void scroll_screen(void);
void update_hardware_cursor(int column, int row);
int	ft_putchar(char c, color_t color);
int	ft_putstr(char *s, color_t color);
int	ft_putnbr(int n, int len, color_t color);
int	ft_printf(color_t color, const char *str, ...);
int	ft_putnbr_exa(unsigned int nbr, char *base, int len, color_t color);
int	ft_putnbr_base_exa(unsigned long long nbr, int len, color_t color);
int	ft_putnbr_unsigned(int n, int len, color_t color);
int ft_atoi(const char *str);

#endif