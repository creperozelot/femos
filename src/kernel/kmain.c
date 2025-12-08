// src/kernel/kmain.c
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "drivers/vga/vga.h"
#include "drivers/pit/pit.h"
#include "drivers/keyboard/keyboard.h"

#include "arch/x86/mm/paging.h"

#include "arch/x86/cpu/gdt/gdt.h" 
#include "arch/x86/cpu/idt/idt.h"
#include "arch/x86/cpu/interrupts.h"
#include "arch/x86/cpu/isr/isr.h"
#include "arch/x86/cpu/irq/irq.h"

#include "kernel/scheduler/scheduler.h"
#include "kernel/memory/kheap.h"
#include "kernel/memory/pmm.h"

#define KHEAP_SIZE (1024 * 1024)

extern uint8_t kernel_end;

static void kputc(char c) {
    vga_putc(c);
}

static void kputs(const char* s) {
    vga_write(s);
}

static void print_dec(int value) {
    char buf[16];
    int i = 0;

    if (value == 0) {
        kputc('0');
        return;
    }
    if (value < 0) {
        kputc('-');
        value = -value;
    }

    while (value > 0 && i < (int)sizeof(buf)) {
        int digit = value % 10;
        buf[i++] = '0' + digit;
        value /= 10;
    }

    while (--i >= 0) {
        kputc(buf[i]);
    }
}

static void print_hex(unsigned int value) {
    const char* hex = "0123456789ABCDEF";
    kputc('0');
    kputc('x');
    for (int shift = 28; shift >= 0; shift -= 4) {
        unsigned int nibble = (value >> shift) & 0xF;
        kputc(hex[nibble]);
    }
}

int printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    for (const char* p = fmt; *p; p++) {
        if (*p != '%') {
            kputc(*p);
            continue;
        }

        p++;
        if (!*p) break;

        switch (*p) {
        case 's': {
            const char* s = va_arg(args, const char*);
            if (s) kputs(s);
            else   kputs("(null)");
            break;
        }
        case 'd':
        case 'i': {
            int v = va_arg(args, int);
            print_dec(v);
            break;
        }
        case 'x': {
            unsigned int v = va_arg(args, unsigned int);
            print_hex(v);
            break;
        }
        case 'c': {
            int c = va_arg(args, int);
            kputc((char)c);
            break;
        }
        case '%':
            kputc('%');
            break;
        default:
            kputc('%');
            kputc(*p);
            break;
        }
    }

    va_end(args);
    return 0;
}

//DEMO TASK

static void task1(void)
{
    int i = 0;
    for (;;) {
        printf("[T1] i=%d\n", i++);
        pit_sleep_ms(500);  // 0.5s
    }
}

static void task2(void)
{
    int j = 0;
    for (;;) {
        printf("    [T2] j=%d\n", j++);
        pit_sleep_ms(1000); // 1s
    }
}


void kmain(void) {
    vga_init();
    printf("kmain start\n");

    uintptr_t kernel_end_phys = (uintptr_t)&kernel_end;

    pmm_init(4 * 1024 * 1024, kernel_end_phys);
    printf("PMM initialized up to kernel_end=0x%x\n", (unsigned)kernel_end_phys);

    paging_init();
    printf("Paging enabled (identity 0–4MiB)\n");

    kheap_init();
    printf("Kernel heap initialized on PMM pages\n");

    gdt_init();
    idt_init();
    isr_install();
    irq_install();

    pit_init(100);
    keyboard_init();

    scheduler_init();
    scheduler_create(task1);
    scheduler_create(task2);

    __asm__ __volatile__("sti");
    scheduler_start();

    for (;;) {
        __asm__ __volatile__("hlt");
    }
}
