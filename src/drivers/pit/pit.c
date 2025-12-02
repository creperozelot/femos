// src/drivers/pit/pit.c
#include "drivers/pit/pit.h"

static inline void outb(uint16_t port, uint8_t value) {
    __asm__ __volatile__("outb %0, %1" : : "a"(value), "Nd"(port));
}

void pit_init(uint32_t freq)
{
    uint32_t divisor = 1193180 / freq;

    extern int printf(const char*, ...);
    printf("PIT: freq=%u, divisor=%u\n", freq, divisor);

    outb(0x43, 0x36); // channel 0, lobyte/hibyte, mode 3, binary
    outb(0x40, (uint8_t)(divisor & 0xFF));
    outb(0x40, (uint8_t)((divisor >> 8) & 0xFF));
}
