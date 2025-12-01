// src/kernel/kmain.c
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "drivers/vga.h"
#include "arch/x86/cpu/gdt.h" 
#include "arch/x86/cpu/idt.h"  // gleich implementieren

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

void kmain(void) {
    vga_init();
    printf("kmain started.\n");

    gdt_init();
    printf("GDT initialized.\n");

    idt_init();
    printf("IDT initialized.\n");

    printf("printf test: %s | %d | %x\n",
           "hello", 1234, 0xBEEF);

    for (;;) {
        __asm__ __volatile__("hlt");
    }
}
