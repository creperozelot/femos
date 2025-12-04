#pragma once

void scheduler_init(void);
int  scheduler_create(void (*entry)(void));
void scheduler_start(void);
void scheduler_yield(void);
