; src/arch/x86/cpu/irq_stubs.asm
bits 32

extern irq_handler

global irq0
global irq1
global irq2
global irq3
global irq4
global irq5
global irq6
global irq7
global irq8
global irq9
global irq10
global irq11
global irq12
global irq13
global irq14
global irq15

; IRQn:
; - CPU pusht eip, cs, eflags, (esp, ss)
; - wir pushen err_code=0 + int_no (32+n)
; - dann geht es in irq_common_stub, der regs_t baut und irq_handler aufruft

%macro IRQ 1
irq%1:
    cli
    push dword 0           ; error code = 0
    push dword (32 + %1)   ; int_no = 32..47 nach PIC-Remap
    jmp irq_common_stub
%endmacro

IRQ 0
IRQ 1
IRQ 2
IRQ 3
IRQ 4
IRQ 5
IRQ 6
IRQ 7
IRQ 8
IRQ 9
IRQ 10
IRQ 11
IRQ 12
IRQ 13
IRQ 14
IRQ 15

; gemeinsamer Stub für alle IRQs
; baut das gleiche regs_t-Layout wie isr_common_stub

irq_common_stub:
    pusha                   ; eax,ecx,edx,ebx,esp,ebp,esi,edi

    push ds
    push es
    push fs
    push gs

    mov ax, 0x10            ; Kernel-Data-Segment aus deiner GDT
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; regs_t* r = (regs_t*)esp;
    push esp
    call irq_handler
    add esp, 4

    pop gs
    pop fs
    pop es
    pop ds

    popa

    add esp, 8              ; int_no + err_code vom Stack entfernen

    sti
    iretd
