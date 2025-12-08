// src/kernel/scheduler/scheduler.h
#pragma once

#include <stdint.h>

void scheduler_init(void);
int  scheduler_create(void (*entry)(void));
void scheduler_start(void);

// Wird von Tasks oder „Warte“-Funktionen (z. B. pit_sleep_ms) aufgerufen
void scheduler_yield(void);

// Wird im PIT-IRQ aufgerufen, um Zeitscheiben zu signalisieren
void scheduler_tick(void);
