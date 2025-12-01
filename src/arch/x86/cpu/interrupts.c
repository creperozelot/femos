// src/arch/x86/cpu/interrupts.c
#include <stdint.h>
#include "arch/x86/cpu/idt.h"
#include "arch/x86/cpu/interrupts.h"

extern void isr_test_stub(void);

void interrupts_init(void)
{
    // Nach gdt_init(): Code-Segment-Selector = 0x08
    uint16_t cs = 0x08;

    idt_set_gate(0x30, (uint32_t)isr_test_stub, cs, 0x8E);
}
