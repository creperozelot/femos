// src/arch/x86/mm/paging.c
#include "arch/x86/mm/paging.h"

static uint32_t page_directory[1024] __attribute__((aligned(4096)));
static uint32_t first_page_table[1024] __attribute__((aligned(4096)));

static inline void load_page_directory(uint32_t* pd)
{
    __asm__ __volatile__("mov %0, %%cr3" :: "r"(pd));
}

static inline void enable_paging(void)
{
    uint32_t cr0;
    __asm__ __volatile__("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000u; // PG-Bit
    __asm__ __volatile__("mov %0, %%cr0" :: "r"(cr0));
}

void paging_init(void)
{
    // Alles nullen
    for (int i = 0; i < 1024; i++) {
        page_directory[i] = 0;
        first_page_table[i] = 0;
    }

    // Identity-Mapping 0–4MiB
    for (int i = 0; i < 1024; i++) {
        uint32_t addr = i * 0x1000;
        first_page_table[i] = addr | 0x3; // Present + RW
    }

    // PDE[0] -> erste Page Table
    page_directory[0] = ((uint32_t)first_page_table) | 0x3; // Present + RW

    load_page_directory(page_directory);
    enable_paging();
}
