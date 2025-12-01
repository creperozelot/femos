; src/arch/x86/cpu/idt_flush.asm
bits 32
global idt_flush

idt_flush:
    mov eax, [esp + 4]
    lidt [eax]
    ret
