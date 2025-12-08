#pragma once
#include <stdint.h>
#include <stddef.h>

void pmm_init(uint32_t mem_size_bytes);
void* pmm_alloc_frame(void);
void pmm_free_frame(void* addr);