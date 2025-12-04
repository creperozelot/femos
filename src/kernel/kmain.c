// src/kernel/kmain.c
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "drivers/vga/vga.h"
#include "drivers/pit/pit.h"
#include "drivers/keyboard/keyboard.h"

#include "arch/x86/cpu/gdt/gdt.h" 
#include "arch/x86/cpu/idt/idt.h"
#include "arch/x86/cpu/interrupts.h"
#include "arch/x86/cpu/isr/isr.h"
#include "arch/x86/cpu/irq/irq.h"

#include "kernel/scheduler/scheduler.h"



// --- printf, basiert auf vga_putc ---

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
        pit_sleep_ms(500);
    }
}

static void task2(void)
{
    int j = 0;
    for (;;) {
        printf("[T2] j=%d\n", j++);
        pit_sleep_ms(1000);
    }
}

void kmain(void) {
    vga_init();
    printf("kmain started.\n");

    gdt_init();
    printf("GDT initialized.\n");

    idt_init();
    printf("IDT base loaded.\n");

    isr_install();
    printf("ISRs (exceptions 0-31) installed.\n");

    irq_install();
    printf("IRQ Installed and Mapped.\n");

    pit_init(100);
    printf("PIT Initialized.\n");

    keyboard_init();
    printf("Keyboard initialized.");

    scheduler_init();
    scheduler_create(task1);
    scheduler_create(task2);
    printf("Tasks created. Enabling interuppts...\n");

    printf("Calling sti() now...\n");
    __asm__ __volatile__("sti");
    printf("sti() executed.\n");

    printf("Starting Scheudler...");
    scheduler_start();

    printf("=========================================\nKernel Init Done.\n=========================================\n");

    for (;;) {
        __asm__ __volatile__("hlt");
    }
}
