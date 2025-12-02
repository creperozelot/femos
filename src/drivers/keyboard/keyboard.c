// src/drivers/keyboard.c
#include <stdint.h>
#include "drivers/keyboard/keyboard.h"
#include "arch/x86/cpu/irq/irq.h"
#include "arch/x86/cpu/isr/isr.h"
#include "drivers/vga/vga.h"

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ __volatile__("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// einfache Scancode-Tabelle (Set 1)
static const char scancode_set1[128] = {
    /* 0x00 */ 0,
    /* 0x01 */ 0,        // ESC
    /* 0x02 */ '1',
    /* 0x03 */ '2',
    /* 0x04 */ '3',
    /* 0x05 */ '4',
    /* 0x06 */ '5',
    /* 0x07 */ '6',
    /* 0x08 */ '7',
    /* 0x09 */ '8',
    /* 0x0A */ '9',
    /* 0x0B */ '0',
    /* 0x0C */ '-',
    /* 0x0D */ '=',
    /* 0x0E */ '\b',     // Backspace
    /* 0x0F */ '\t',     // Tab
    /* 0x10 */ 'q',
    /* 0x11 */ 'w',
    /* 0x12 */ 'e',
    /* 0x13 */ 'r',
    /* 0x14 */ 't',
    /* 0x15 */ 'z',      // QWERTZ
    /* 0x16 */ 'u',
    /* 0x17 */ 'i',
    /* 0x18 */ 'o',
    /* 0x19 */ 'p',
    /* 0x1A */ '[',
    /* 0x1B */ ']',
    /* 0x1C */ '\n',     // Enter
    /* 0x1D */ 0,        // Ctrl
    /* 0x1E */ 'a',
    /* 0x1F */ 's',
    /* 0x20 */ 'd',
    /* 0x21 */ 'f',
    /* 0x22 */ 'g',
    /* 0x23 */ 'h',
    /* 0x24 */ 'j',
    /* 0x25 */ 'k',
    /* 0x26 */ 'l',
    /* 0x27 */ ';',
    /* 0x28 */ '\'',
    /* 0x29 */ '`',
    /* 0x2A */ 0,        // Left Shift
    /* 0x2B */ '\\',
    /* 0x2C */ 'y',
    /* 0x2D */ 'x',
    /* 0x2E */ 'c',
    /* 0x2F */ 'v',
    /* 0x30 */ 'b',
    /* 0x31 */ 'n',
    /* 0x32 */ 'm',
    /* 0x33 */ ',',
    /* 0x34 */ '.',
    /* 0x35 */ '/',
    /* 0x36 */ 0,        // Right Shift
    /* 0x37 */ '*',
    /* 0x38 */ 0,        // Alt
    /* 0x39 */ ' ',      // Space
    /* 0x3A */ 0,        // CapsLock
    /* Rest 0 */
};

static void keyboard_callback(regs_t* r)
{
    (void)r;

    uint8_t scancode = inb(0x60);

    // Key-Release ignorieren (High-Bit gesetzt)
    if (scancode & 0x80) {
        return;
    }

    char c = 0;
    if (scancode < sizeof(scancode_set1)) {
        c = scancode_set1[scancode];
    }

    if (!c) {
        return;
    }

    if (c == '\n') {
        vga_putc('\n');
    } else if (c == '\b') {
        // sehr einfache Backspace-Logik:
        vga_putc('\b');
    } else if (c == '\t') {
        // Tab -> 4 Spaces
        vga_putc(' ');
        vga_putc(' ');
        vga_putc(' ');
        vga_putc(' ');
    } else {
        vga_putc(c);
    }
}

void keyboard_init(void)
{
    irq_install_handler(1, keyboard_callback);
}
