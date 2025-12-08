#pragma once
#include <stddef.h>
#include <stdint.h>

void   kheap_init(void* heap_start, size_t heap_size);

void*  kmalloc(size_t size);
void   kfree(void* ptr);

void*  kcalloc(size_t nmemb, size_t size);
void*  krealloc(void* ptr, size_t new_size);