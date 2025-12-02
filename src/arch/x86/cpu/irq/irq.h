// src/arch/x86/cpu/irq/irq.h
#pragma once
#include <stdint.h>
#include "arch/x86/cpu/isr/isr.h"

void irq_install(void);
void irq_handler(regs_t* r);
void irq_install_handler(int irq, void (*handler)(regs_t* r));
void irq_uninstall_handler(int irq);
