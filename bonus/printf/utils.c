#include "ft_printf.h"

int	ft_putchar(char c, color_t color)
{
	terminal_putchar(c, color);
	return (1);
}

void terminal_write_string(const char* data, color_t color) {
    for (int i = 0; data[i] != '\0'; i++) {
        terminal_putchar(data[i], color);
    }  
}

int	ft_putnbr(int n, int len, color_t color)
{
	if (n == INT_MIN)
	{
		ft_putstr("-2147483648", color);
		len += 11;
	}
	if (n > INT_MIN && n <= INT_MAX)
	{
		if (n < 10 && n >= 0)
		{
			n = n + 48;
			ft_putchar(n, color);
		}
		else if (n < 0)
		{
			ft_putchar('-', color);
			len = ft_putnbr(n * -1, len, color);
		}
		else
		{
			len = ft_putnbr(n / 10, len, color);
			ft_putnbr(n % 10, len, color);
		}
		len++;
	}
	return (len);
}

int	ft_write_exa(unsigned long long nbr, char *base, int len, color_t color)
{
	if (nbr < 16)
		terminal_putchar(base[nbr], color);
	else
	{
		len = ft_write_exa(nbr / 16, base, len, color);
		ft_write_exa(nbr % 16, base, len, color);
	}
	len++;
	return (len);
}

int	ft_putnbr_base_exa(unsigned long long nbr, int len, color_t color)
{
	terminal_write_string("0x", color);
	len += 2;
	len = ft_write_exa(nbr, EXA_DOWN, len, color);
	return (len);
}

int	ft_putnbr_exa(unsigned int nbr, char *base, int len, color_t color)
{
	if (nbr < 16)
		terminal_putchar(base[nbr], color);
	else
	{
		len = ft_putnbr_exa(nbr / 16, base, len, color);
		ft_putnbr_exa(nbr % 16, base, len, color);
	}
	return (++len);
}

int	ft_putnbr_unsigned(int n, int len, color_t color)
{
	unsigned long int	nbis;

	nbis = (unsigned)n;
	if (nbis < 10)
	{
		nbis = nbis + 48;
		ft_putchar(nbis, color);
	}
	else
	{
		len = ft_putnbr(nbis / 10, len, color);
		ft_putnbr(nbis % 10, len, color);
	}
	return (++len);
}

int	ft_putstr(char *s, color_t color)
{
	int	i;

	i = 0;
	if (s == NULL)
	{
		terminal_write_string("null", color);
		return (i += 6);
	}
	while (s[i])
	{
		i += ft_putchar(s[i], color);
	}
	return (i);
}

int	ft_atoi(const char *str)
{
	int				i;
	int				a;
	long long int	nb;

	i = 0;
	a = 1;
	nb = 0;
	while ((str[i] >= 9 && str[i] <= 13) || str[i] == 32)
		i++;
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
			a = a * -1;
		i++;
	}
	while (str[i] >= '0' && str[i] <= '9')
	{
		nb = nb * 10 + str[i] - '0';
		i++;
	}
	return ((int)nb * a);
}