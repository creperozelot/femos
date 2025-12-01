// src/drivers/vga.c
#include "drivers/vga.h"

#define VGA_WIDTH  80
#define VGA_HEIGHT 25
#define VGA_MEM    ((uint16_t*)0xB8000)
#define COLOR      0x07  // hellgrau auf schwarz

static size_t cursor_row = 0;
static size_t cursor_col = 0;

static void vga_putentry_at(char c, uint8_t color, size_t x, size_t y) {
    const size_t index = y * VGA_WIDTH + x;
    VGA_MEM[index] = ((uint16_t)color << 8) | (uint8_t)c;
}

void vga_clear(void) {
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            vga_putentry_at(' ', COLOR, x, y);
        }
    }
    cursor_row = 0;
    cursor_col = 0;
}

void vga_init(void) {
    vga_clear();
}

void vga_putc(char c) {
    if (c == '\n') {
        cursor_col = 0;
        if (++cursor_row >= VGA_HEIGHT) {
            cursor_row = 0; // noch kein Scrollen
        }
        return;
    }

    vga_putentry_at(c, COLOR, cursor_col, cursor_row);
    if (++cursor_col >= VGA_WIDTH) {
        cursor_col = 0;
        if (++cursor_row >= VGA_HEIGHT) {
            cursor_row = 0;
        }
    }
}

void vga_write(const char* s) {
    while (*s) {
        vga_putc(*s++);
    }
}
