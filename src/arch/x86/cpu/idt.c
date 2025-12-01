// src/arch/x86/cpu/idt.c
#include "arch/x86/cpu/idt.h"

#define IDT_ENTRIES 256

static struct idt_entry idt[IDT_ENTRIES];
static struct idt_ptr   idt_descriptor;

extern void idt_flush(uint32_t);

void idt_set_gate(int num, uint32_t base, uint16_t sel, uint8_t flags)
{
    idt[num].base_low  = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].sel       = sel;
    idt[num].always0   = 0;
    idt[num].flags     = flags;
}

void idt_init(void)
{
    idt_descriptor.base  = (uint32_t)&idt;
    idt_descriptor.limit = (sizeof(struct idt_entry) * IDT_ENTRIES) - 1;

    for (int i = 0; i < IDT_ENTRIES; i++) {
        idt_set_gate(i, 0, 0, 0);
    }

    idt_flush((uint32_t)&idt_descriptor);
}
