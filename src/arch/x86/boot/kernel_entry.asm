; src/arch/x86/boot/kernel_entry.asm
bits 32

section .multiboot
align 4
    dd 0x1BADB002
    dd 0x0
    dd -(0x1BADB002+0x0)

section .text
global start
extern kmain

start:
    cli

    mov dword [0xB8000], 0x07450045   ; 'E' anzeigen

    call kmain

.hang:
    hlt
    jmp .hang
