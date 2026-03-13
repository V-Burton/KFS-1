global header_start
section .bss
align 16
stack_bottom:
    resb 16384 ; 16KB stack
stack_top:

section .text
global start
extern kernel_main; Cette fonction est externe

start:
    cli ; 
    ; Set up the stack
    mov esp, stack_top ;
    ; Call the kernel's main function
    call kernel_main

.hang:
    hlt
    jmp .hang    ; ← si hlt est interrompu, on boucle