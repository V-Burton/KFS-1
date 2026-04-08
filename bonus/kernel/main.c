#include <stdint.h>
#include "gdt.h"
#include "idt.h"
#include "../printf/ft_printf.h"

void kernel_main() {
    flush_screen();
    init_gdt();
    setup_idt();
    ft_printf(0x0A, "Commandes disponibles :\n");
    ft_printf(0x0A, "  proc  - Affiche les informations CPU\n");
    ft_printf(0x0A, "  clear - Efface l'ecran\n");
    ft_printf(0x0A, "  addr_gdt - Affiche l'adresse de la GDT\n");
    ft_printf(0x0A, "  help  - Affiche cette aide\n");
    ft_printf(0x0E, "KFS>");
}