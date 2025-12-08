// src/kernel/memory/pmm.h
#pragma once
#include <stdint.h>
#include <stddef.h>

// mem_size_bytes: wie viel physischer RAM soll verwaltet werden
// kernel_end_phys: physische Adresse hinter dem Kernel (aus Linker)
void   pmm_init(uint32_t mem_size_bytes, uintptr_t kernel_end_phys);
void*  pmm_alloc_frame(void);
void   pmm_free_frame(void* addr);
