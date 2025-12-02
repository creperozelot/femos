; src/arch/x86/cpu/gdt/gdt_flush.asm
bits 32
global gdt_flush

; void gdt_flush(uint32_t gdt_ptr_addr);

gdt_flush:
    mov eax, [esp + 4]
    lgdt [eax]

    mov ax, 0x10          ; Data-Segment (Eintrag 2 -> 2*8 = 0x10)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    jmp 0x08:.flush_label ; Code-Segment (Eintrag 1 -> 0x08)

.flush_label:
    ret
