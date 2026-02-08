section .multiboot
align 4
    dd 0x1BADB002              ; Magic Number
    dd 0x00                    ; Flags
    dd -(0x1BADB002 + 0x00)    ; Checksum

section .text
global _start
extern kernel_main

_start:
    mov esp, stack_top         ; Configura a pilha
    call kernel_main           ; Pula para o C
    cli
.hang:
    hlt
    jmp .hang

section .bss
align 16
stack_bottom:
    resb 16384                 ; 16KB de Stack
stack_top:
