; src/arch/x86/cpu/isr/isr_stubs.asm
bits 32

; Exportiere nur isr0..isr31 + isr_common_stub
global isr0
global isr1
global isr2
global isr3
global isr4
global isr5
global isr6
global isr7
global isr8
global isr9
global isr10
global isr11
global isr12
global isr13
global isr14
global isr15
global isr16
global isr17
global isr18
global isr19
global isr20
global isr21
global isr22
global isr23
global isr24
global isr25
global isr26
global isr27
global isr28
global isr29
global isr30
global isr31

global isr_common_stub

extern isr_handler

; Makros für Exceptions ohne / mit Error-Code

%macro ISR_NOERR 1
isr%1:
    cli
    push dword 0        ; Fake error code
    push dword %1       ; interrupt number
    jmp isr_common_stub
%endmacro

%macro ISR_ERR 1
isr%1:
    cli
    push dword %1       ; interrupt number (Error-Code liegt schon auf dem Stack)
    jmp isr_common_stub
%endmacro

; Exceptions ohne Error-Code
ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_NOERR 9
ISR_NOERR 15
ISR_NOERR 16
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_NOERR 21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_NOERR 29
ISR_NOERR 30
ISR_NOERR 31

; Exceptions MIT Error-Code: 8, 10–14, 17
ISR_ERR   8
ISR_ERR   10
ISR_ERR   11
ISR_ERR   12
ISR_ERR   13
ISR_ERR   14
ISR_ERR   17

; Gemeinsamer ISR-Stub, passt zu deinem regs_t in isr.h

isr_common_stub:
    pusha                   ; eax,ecx,edx,ebx,esp,ebp,esi,edi

    push ds
    push es
    push fs
    push gs

    mov ax, 0x10            ; Kernel-Data-Segment (GDT)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp                ; regs_t* r
    call isr_handler
    add esp, 4

    pop gs
    pop fs
    pop es
    pop ds

    popa

    add esp, 8              ; int_no + err_code

    sti
    iretd
