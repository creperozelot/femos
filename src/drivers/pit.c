// src/drivers/pit.c
#include "drivers/pit.h"
#include "arch/x86/cpu/irq.h"

static uint32_t tick = 0;

static inline void outb(uint16_t port, uint8_t value) {
    __asm__ __volatile__("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ __volatile__("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static void pit_callback(regs_t* r)
{
    (void)r;
    tick++;
    if (tick % 100 == 0) {
        extern int printf(const char*, ...);
        printf("[PIT] tick=%d\n", tick);
    }
}

void pit_init(uint32_t freq)
{
    uint32_t divisor = 1193180 / freq;

    irq_install_handler(0, pit_callback);

    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, divisor >> 8);
}
