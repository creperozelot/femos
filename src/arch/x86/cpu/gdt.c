// src/arch/x86/cpu/gdt.c
#include "arch/x86/cpu/gdt.h"

#define GDT_ENTRIES 3

static struct gdt_entry gdt[GDT_ENTRIES];
static struct gdt_ptr   gdt_descriptor;

extern void gdt_flush(uint32_t); // ASM-Funktion

static void gdt_set_gate(int num, uint32_t base, uint32_t limit,
                         uint8_t access, uint8_t gran)
{
    gdt[num].base_low    = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high   = (base >> 24) & 0xFF;

    gdt[num].limit_low   = (limit & 0xFFFF);
    gdt[num].granularity = (limit >> 16) & 0x0F;

    gdt[num].granularity |= gran & 0xF0;
    gdt[num].access      = access;
}

void gdt_init(void)
{
    gdt_descriptor.limit = (sizeof(struct gdt_entry) * GDT_ENTRIES) - 1;
    gdt_descriptor.base  = (uint32_t)&gdt;

    // Null-Descriptor
    gdt_set_gate(0, 0, 0, 0, 0);

    // Code-Segment: base=0, limit=4GB, ring0, code
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

    // Data-Segment: base=0, limit=4GB, ring0, data
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    gdt_flush((uint32_t)&gdt_descriptor);
}
