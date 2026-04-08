# ifndef MICROSHELL_H
# define MICROSHELL_H

# include "../printf/ft_printf.h"

struct keyboard_state {
    int shift;
};

typedef struct keyboard_state keyboard_state_t;


void evaluate_command(const char *cmd);
void handle_keyboard(uint8_t scancode);

#endif