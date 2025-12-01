; src/arch/x86/cpu/gdt_flush.asm
bits 32
global gdt_flush

; void gdt_flush(uint32_t gdt_ptr_addr);

gdt_flush:
    mov eax, [esp + 4]    ; Adresse von struct gdt_ptr
    lgdt [eax]

    mov ax, 0x10          ; Data-Segment-Selector (Index 2 -> 2*8 = 0x10)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    jmp 0x08:.flush_label ; Far jump in Code-Segment (Index 1 -> 0x08)

.flush_label:
    ret
