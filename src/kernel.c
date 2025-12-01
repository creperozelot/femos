// src/kernel.c
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#define VGA_WIDTH   80
#define VGA_HEIGHT  25
#define VGA_MEM     ((uint16_t*)0xBB000)
#define COLOR       0x07

static size_t cursor_row = 0;
static size_t cursor_col = 0;

static void vga_putentry_at(char c, uint8_t color, size_t x, size_t y) {
    const size_t index = y * VGA_WIDTH  + x;
    VGA_MEM[index] = ((uint16_t)color << 8) | (uint8_t)c;
}

static void clear_screen(void) {
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            vga_putentry_at(' ', COLOR, x, y);
        }
    }
    cursor_row = 0;
    cursor_col = 0;
}

static void putchar(char c) {
    if (c == '\n')
    {
        cursor_col = 0;
        if (++cursor_row >= VGA_HEIGHT)
        {
            cursor_row = 0;
        }
        return;
    }

    vga_putentry_at(c, COLOR, cursor_col, cursor_row);
    if (++cursor_col >= VGA_WIDTH)
    {
        cursor_col = 0;
        if (++cursor_row >= VGA_HEIGHT)
        {
            cursor_row = 0;
        }
    }
}

static void puts(const char* s) {
    while (*s) 
    {
        putchar(*s++);
    }
}

static void print_dec(int value) {
    char buf[16];
    int i = 0;

    if (value == 0)
    {
        putchar('0');
        return;
    }

    if (value < 0)
    {
        putchar('-');
        value = -value;
    }

    while (value > 0 && i < (int)sizeof(buf))
    {
        int digit = value % 10;
        buf[i++] = '0' + digit;
        value /= 10;
    
    }
    
    while (--i >= 0)
    {
        putchar(buf[i]);
    }
}

static void print_hex(unsigned int value) {
    const char* hex = "0123456789ABCDEF";
    putchar('0');
    putchar('x');

    for (int shift = 28; shift >= 0; shift -= 4)
    {
        unsigned int nibble = (value >> shift) & 0xF;
        putchar(hex[nibble]);
    }
}

int printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    for (const char* p = fmt; *p; p++)
    {
        if (*p != '%')
        {
            putchar(*p);
            continue;
        }

        p++;
        if (!*p) break;

        switch (*p)
        {
        case 's': {
            const char* s = va_arg(args, const char*);
            if (s)
            {
                puts(s);
            }
            else
            {
                puts("(null)");
            }
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
            putchar((char)c);
            break;
        }

        case '%':
            putchar('%');
            break;
        
        default:
            putchar('%');
            putchar(*p);
            break;
        }
    }

    va_end(args);
    return 0;
}

void kmain(void) {
    clear_screen();
    printf("Welcome to my 32-bit x86 kernel!\n");
    printf("This is a basic printf: %s\n", "Hello World");
    printf("Decimal: %d  Hex: %x\n", 1234, 1234);

    for (;;) {
        __asm__ __volatile__("hlt");
    }
}