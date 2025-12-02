// src/arch/x86/cpu/isr.c
#include <stdint.h>
#include "arch/x86/cpu/idt/idt.h"
#include "arch/x86/cpu/isr/isr.h"

// printf aus deinem Kernel
extern int printf(const char* fmt, ...);

extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);

static const char* exception_messages[32] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void isr_install(void)
{
    uint16_t cs = 0x08; // Dein Kernel-Code-Segment

    idt_set_gate(0,  (uint32_t)isr0,  cs, 0x8E);
    idt_set_gate(1,  (uint32_t)isr1,  cs, 0x8E);
    idt_set_gate(2,  (uint32_t)isr2,  cs, 0x8E);
    idt_set_gate(3,  (uint32_t)isr3,  cs, 0x8E);
    idt_set_gate(4,  (uint32_t)isr4,  cs, 0x8E);
    idt_set_gate(5,  (uint32_t)isr5,  cs, 0x8E);
    idt_set_gate(6,  (uint32_t)isr6,  cs, 0x8E);
    idt_set_gate(7,  (uint32_t)isr7,  cs, 0x8E);
    idt_set_gate(8,  (uint32_t)isr8,  cs, 0x8E);
    idt_set_gate(9,  (uint32_t)isr9,  cs, 0x8E);
    idt_set_gate(10, (uint32_t)isr10, cs, 0x8E);
    idt_set_gate(11, (uint32_t)isr11, cs, 0x8E);
    idt_set_gate(12, (uint32_t)isr12, cs, 0x8E);
    idt_set_gate(13, (uint32_t)isr13, cs, 0x8E);
    idt_set_gate(14, (uint32_t)isr14, cs, 0x8E);
    idt_set_gate(15, (uint32_t)isr15, cs, 0x8E);
    idt_set_gate(16, (uint32_t)isr16, cs, 0x8E);
    idt_set_gate(17, (uint32_t)isr17, cs, 0x8E);
    idt_set_gate(18, (uint32_t)isr18, cs, 0x8E);
    idt_set_gate(19, (uint32_t)isr19, cs, 0x8E);
    idt_set_gate(20, (uint32_t)isr20, cs, 0x8E);
    idt_set_gate(21, (uint32_t)isr21, cs, 0x8E);
    idt_set_gate(22, (uint32_t)isr22, cs, 0x8E);
    idt_set_gate(23, (uint32_t)isr23, cs, 0x8E);
    idt_set_gate(24, (uint32_t)isr24, cs, 0x8E);
    idt_set_gate(25, (uint32_t)isr25, cs, 0x8E);
    idt_set_gate(26, (uint32_t)isr26, cs, 0x8E);
    idt_set_gate(27, (uint32_t)isr27, cs, 0x8E);
    idt_set_gate(28, (uint32_t)isr28, cs, 0x8E);
    idt_set_gate(29, (uint32_t)isr29, cs, 0x8E);
    idt_set_gate(30, (uint32_t)isr30, cs, 0x8E);
    idt_set_gate(31, (uint32_t)isr31, cs, 0x8E);
}

void isr_handler(regs_t* r)
{
    uint32_t n = r->int_no;

    printf("\n[EXCEPTION] #%u: ", n);
    if (n < 32) {
        printf("%s", exception_messages[n]);
    } else {
        printf("Unknown");
    }

    printf("  err=0x%x  eip=0x%x  cs=0x%x\n",
           r->err_code, r->eip, r->cs);
    printf("System halted.\n");

    for (;;) {
        __asm__ __volatile__("hlt");
    }
}
