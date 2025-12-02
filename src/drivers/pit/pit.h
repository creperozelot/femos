// src/drivers/pit/pit.h
#pragma once
#include <stdint.h>

void pit_init(uint32_t frequency);

uint32_t pit_get_frequency(void);
uint32_t pit_get_ticks(void);

uint32_t pit_uptime_ms(void);
uint32_t pit_uptime_seconds(void);
void pit_sleep_ms(uint32_t ms);