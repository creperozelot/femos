// src/drivers/pit/pit.c
#include "drivers/pit/pit.h"
#include "arch/x86/cpu/irq/irq.h"
#include "arch/x86/cpu/isr/isr.h"
#include "kernel/scheduler/scheduler.h"

static volatile uint32_t pit_ticks = 0;
static uint32_t pit_frequency = 0;

static inline void outb(uint16_t port, uint8_t value) {
    __asm__ __volatile__("outb %0, %1" : : "a"(value), "Nd"(port));
}

static void pit_callback(regs_t* r)
{
    (void)r;
    pit_ticks++;

    // alle 10 Ticks (~100ms bei 100Hz) Zeitscheibe
    if (pit_ticks % 10 == 0) {
        scheduler_tick();
    }
}


void pit_init(uint32_t freq)
{
    pit_frequency = freq;

    // IRQ0-Handler registrieren
    irq_install_handler(0, pit_callback);

    // PIT programmieren (Channel 0, Mode 3)
    uint32_t divisor = 1193180 / freq;
    if (divisor == 0) {
        divisor = 1;
    }

    outb(0x43, 0x36);                      // channel 0, lobyte/hibyte, mode 3, binary
    outb(0x40, (uint8_t)(divisor & 0xFF));
    outb(0x40, (uint8_t)((divisor >> 8) & 0xFF));
}

uint32_t pit_get_frequency(void)
{
    return pit_frequency;
}

uint32_t pit_get_ticks(void)
{
    // für uns reicht ein nicht-atomarer Read
    return pit_ticks;
}

uint32_t pit_uptime_ms(void)
{
    if (pit_frequency == 0) return 0;

    uint32_t t = pit_get_ticks();
    // Achtung: t * 1000 kann irgendwann überlaufen, ist aber für Hobby-Kernel ok
    return (t * 1000U) / pit_frequency;
}

uint32_t pit_uptime_seconds(void)
{
    if (pit_frequency == 0) return 0;

    uint32_t t = pit_get_ticks();
    return t / pit_frequency;
}

void pit_sleep_ms(uint32_t ms)
{
    if (pit_frequency == 0) return;

    uint32_t start  = pit_get_ticks();
    uint32_t delta  = (ms * pit_frequency) / 1000U;
    if (delta == 0) delta = 1;
    uint32_t target = start + delta;

    while ((int32_t)(pit_get_ticks() - target) < 0) {
        // Scheduler bekommt die Chance zu wechseln, falls der Timer das verlangt
        scheduler_yield();
    }
}
