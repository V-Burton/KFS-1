#include <stdint.h>
#include "gdt.h"

gdt_ptr_t gdt_ptr;
gdt_entry_t *gdt_ptr_entries = (gdt_entry_t *)0x00000800;

void gdt_set_gate(gdt_entry_t *gdt_ptr_entries, int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt_ptr_entries[num].base_low = (base & 0xFFFF);
    gdt_ptr_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_ptr_entries[num].base_high = (base >> 24) & 0xFF;
    gdt_ptr_entries[num].limit_low = (limit & 0xFFFF);
    gdt_ptr_entries[num].granularity = (gran << 4) | ((limit >> 16) & 0x0F); // fusion des 4 bits de flags et les 4 bit de limit haut voir https://wiki.osdev.org/Global_Descriptor_Table
    gdt_ptr_entries[num].access = access;
}

void init_gdt() {
    gdt_ptr.limit = (sizeof(gdt_entry_t) * 7) -1;
    gdt_ptr.base = (uint32_t)gdt_ptr_entries;

    gdt_set_gate(gdt_ptr_entries, 0, 0, 0, 0, 0);             // Null segment
    gdt_set_gate(gdt_ptr_entries, 1, 0, 0xFFFFF, 0x9B, 0x0c); // Code segment
    gdt_set_gate(gdt_ptr_entries, 2, 0, 0xFFFFF, 0x93, 0x0c); // Data segment
    gdt_set_gate(gdt_ptr_entries, 3, 0, 0xFFFFF, 0x93, 0x0c); // stack segment
    gdt_set_gate(gdt_ptr_entries, 4, 0, 0xFFFFF, 0xFA, 0x0c); // code segment pour les applications en user mode
    gdt_set_gate(gdt_ptr_entries, 5, 0, 0xFFFFF, 0xF2, 0x0c); // data segment pour les applications en user mode
    gdt_set_gate(gdt_ptr_entries, 6, 0, 0xFFFFF, 0xF2, 0x0c); // stack segment pour les applications en user mode
    // -> Data segment et stack segment sont techniquement identique et donc je pourrais utiliser le meme segment pour les deux.

    gdt_flush((uint32_t)&gdt_ptr);
}

