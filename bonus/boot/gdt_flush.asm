[GLOBAL gdt_flush]

gdt_flush:
    mov eax, [esp + 4]  ; récupère l'addresse de gdt_ptr passée em argument
    lgdt [eax]          ; Charger la GDT

    ; On recharge les segments de données https://stackoverflow.com/questions/23978486/far-jump-in-gdt-in-bootloader
    mov ax, 0x10         ; 0x10 est le sélecteur Kernel Data (index 2 * 8)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Far jump pour recharger CS (Kernel code = 0x08)
    ; On utilise un far jump pour valider le nouveau segment de code
    jmp 0x08:.flush_cs

.flush_cs:
    ret