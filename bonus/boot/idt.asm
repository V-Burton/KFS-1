[GLOBAL common_stub]
extern interrupt_handler

global irq1 ; Clavier (33)
irq1:
    push dword 0   ; Code d'erreur fictif
    push dword 33  ; Numéro de l'interruption
    jmp common_stub

common_stub:
    pushad        ; Sauvegarde tous les registres (EAX, ECX, etc.)
    
    push esp      ; Passe l'adresse de la pile actuelle au debut de la structure
    cld           ; Clear Direction Flag pour les opérations de chaîne
    call interrupt_handler
    add esp, 4    ;
    
    popad         ; Restaure tout
    add esp, 8    ; Nettoie le numéro d'interruption et le code d'erreur
  
    iret

global isr8 ; Double Fault (8)
isr8:
    push dword 8
    jmp common_stub

global isr13 ; General Protection Fault (13)
isr13:
    push dword 0
    push dword 13
    jmp common_stub

global isr14
isr14:          ; Page fault - CPU pousse un error code
    push dword 14
    jmp common_stub

global isr_default
isr_default:
    iret