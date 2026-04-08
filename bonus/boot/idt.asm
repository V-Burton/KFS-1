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

global isr0 ; Division par zéro (0)
isr0:
    push dword 0
    jmp common_stub

global isr1 ; Debug (1)
isr1:
    push dword 1
    jmp common_stub 

global isr2 ; Non Maskable Interrupt (2)
isr2:
    push dword 2
    jmp common_stub

global isr3 ; Breakpoint (3)
isr3:
    push dword 3
    jmp common_stub
global isr4 ; Overflow (4)
isr4:
    push dword 4
    jmp common_stub

global isr5 ; Bound Range Exceeded (5)
isr5:
    push dword 5
    jmp common_stub

global isr6 ; Invalid Opcode (6)
isr6:
    push dword 6
    jmp common_stub

global isr7 ; Device Not Available (7)
isr7:
    push dword 7
    jmp common_stub

global isr8 ; Double Fault (8)
isr8:
    push dword 8
    jmp common_stub

global isr9 ; Coprocessor Segment Overrun (9)
isr9:
    push dword 9
    jmp common_stub

global isr10 ; Invalid TSS (10)
isr10:
    push dword 10
    jmp common_stub

global isr11 ; Segment Not Present (11)
isr11:
    push dword 11
    jmp common_stub

global isr12 ; Stack-Segment Fault (12)
isr12:
    push dword 12
    jmp common_stub

global isr13 ; General Protection Fault (13)
isr13:
    push dword 13
    jmp common_stub
global isr14
isr14:          ; Page fault - CPU pousse un error code
    push dword 14
    jmp common_stub
global isr15 ; Reserved (15)
isr15:
    push dword 15
    jmp common_stub
global isr16 ; x87 Floating-Point Exception (16)
isr16:
    push dword 16
    jmp common_stub
global isr17 ; Alignment Check (17)
isr17:
    push dword 17
    jmp common_stub
global isr18 ; Machine Check (18)
isr18:
    push dword 18
    jmp common_stub
global isr19 ; SIMD Floating-Point Exception (19)
isr19:
    push dword 19
    jmp common_stub
global isr20 ; Virtualization Exception (20)
isr20:
    push dword 20
    jmp common_stub
global isr21 ; Control Protection Exception (21)
isr21:
    push dword 21
    jmp common_stub

global isr_reserved ; Interruption réservée (22-31)
isr_reserved:
    push dword 0xFF ; Code d'erreur fictif pour les interruptions inconnues
    jmp common_stub
global isr_default ; Interruption inconnue (32-255)
isr_default:
    push dword 0xFF ; Code d'erreur fictif pour les interruptions inconnues
    jmp common_stub