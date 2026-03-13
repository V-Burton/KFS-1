[GLOBAL common_stub]
extern interrupt_handler
global isr0

isr0:
    push dword 0   ; Code d'erreur fictif
    push dword 0   ; Numéro de l'interruption
    jmp common_stub

; global irq1 ; Clavier (33)
; irq1:
;     push dword 0   ; Code d'erreur fictif
;     push dword 33  ; Numéro de l'interruption
;     jmp common_stub

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

extern keyboard_handler

global irq1
irq1:
    push eax
    
    ; Lire le scancode
    in al, 0x60
    
    ; Écrire directement en mémoire VGA, pas de C du tout
    mov byte [0xB8000], al
    mov byte [0xB8001], 0x4F   ; blanc sur rouge
    
    ; EOI au PIC master
    mov al, 0x20
    out 0x20, al
    
    pop eax
    iret

global isr_default
isr_default:
    iret