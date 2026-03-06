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
    ; Set up the stack
    mov esp, stack_top

    ; Call the kernel's main function
    call kernel_main

    ; If kernel_main returns, halt the CPU
    hlt