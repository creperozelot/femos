; src/kernel.asm
bits 32

section .multiboot
align 4
    dd 0x1BADB002           ; magic (Multiboot)
    dd 0x0                  ; flags
    dd -(0x1BADB002+0x0)    ; checksum: magic + flags + checksum = 0

section .text
global start
extern kmain

start:
    cli
    call kmain

.hang:
    hlt
    jmp .hang
