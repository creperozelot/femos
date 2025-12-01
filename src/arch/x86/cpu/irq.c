// src/arch/x86/cpu/irq.c
#include "arch/x86/cpu/irq.h"
#include "arch/x86/cpu/idt.h"
#include "arch/x86/cpu/isr.h"

#define MASTER_PIC_CMD  0x20
#define MASTER_PIC_DATA 0x21
#define SLAVE_PIC_CMD   0xA0
#define SLAVE_PIC_DATA  0xA1
#define PIC_EOI         0x20

static void (*irq_routines[16])(regs_t* r) = { 0 };

static inline void outb(uint16_t port, uint8_t value) {
    __asm__ __volatile__("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ __volatile__("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void irq_install_handler(int irq, void (*handler)(regs_t* r)) {
    irq_routines[irq] = handler;
}

void irq_uninstall_handler(int irq) {
    irq_routines[irq] = 0;
}

static void pic_remap(void) {
    uint8_t a1 = inb(MASTER_PIC_DATA);
    uint8_t a2 = inb(SLAVE_PIC_DATA);

    // Initialisieren (ICW1)
    outb(MASTER_PIC_CMD, 0x11);
    outb(SLAVE_PIC_CMD,  0x11);

    // ICW2: Interrupt Offsets
    outb(MASTER_PIC_DATA, 0x20); // Master PIC -> 32–39
    outb(SLAVE_PIC_DATA,  0x28); // Slave PIC  -> 40–47

    // ICW3: Verkabelung
    outb(MASTER_PIC_DATA, 4);    // Master hat Slave an IRQ2
    outb(SLAVE_PIC_DATA,  2);    // Slave hängt an IRQ2

    // ICW4
    outb(MASTER_PIC_DATA, 0x01);
    outb(SLAVE_PIC_DATA,  0x01);

    // Masken zurücksetzen
    outb(MASTER_PIC_DATA, a1);
    outb(SLAVE_PIC_DATA,  a2);
}

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

void irq_install(void)
{
    uint16_t cs = 0x08;

    pic_remap();

    idt_set_gate(32, (uint32_t)irq0,  cs, 0x8E);
    idt_set_gate(33, (uint32_t)irq1,  cs, 0x8E);
    idt_set_gate(34, (uint32_t)irq2,  cs, 0x8E);
    idt_set_gate(35, (uint32_t)irq3,  cs, 0x8E);
    idt_set_gate(36, (uint32_t)irq4,  cs, 0x8E);
    idt_set_gate(37, (uint32_t)irq5,  cs, 0x8E);
    idt_set_gate(38, (uint32_t)irq6,  cs, 0x8E);
    idt_set_gate(39, (uint32_t)irq7,  cs, 0x8E);

    idt_set_gate(40, (uint32_t)irq8,  cs, 0x8E);
    idt_set_gate(41, (uint32_t)irq9,  cs, 0x8E);
    idt_set_gate(42, (uint32_t)irq10, cs, 0x8E);
    idt_set_gate(43, (uint32_t)irq11, cs, 0x8E);
    idt_set_gate(44, (uint32_t)irq12, cs, 0x8E);
    idt_set_gate(45, (uint32_t)irq13, cs, 0x8E);
    idt_set_gate(46, (uint32_t)irq14, cs, 0x8E);
    idt_set_gate(47, (uint32_t)irq15, cs, 0x8E);
}

void irq_handler(regs_t* r)
{
    int irq = r->int_no - 32;

    if (irq < 0 || irq > 15)
        return;

    if (irq_routines[irq])
        irq_routines[irq](r);

    // Master-PIC EOI
    outb(MASTER_PIC_CMD, PIC_EOI);

    // Slave-PIC EOI
    if (irq >= 8)
        outb(SLAVE_PIC_CMD, PIC_EOI);
}
