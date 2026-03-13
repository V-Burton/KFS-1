#ifndef GDT_H
#define GDT_H

// Structure d'une entrée de la GDT (8 octets)
struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed));

// Pointeur GDT pour l'instruction LGDT (6 octets)
struct gdt_ptr_struct {
    uint16_t limit; // Taille de la gdt - 1
    uint32_t base; // Addresse du premier element de la gdt
} __attribute__((packed));

typedef struct gdt_entry gdt_entry_t;
typedef struct gdt_ptr_struct gdt_ptr_t;

void init_gdt();

extern void gdt_flush(uint32_t gdt_ptr_addr);

#endif