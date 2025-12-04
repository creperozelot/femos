#include "kernel/memory/kheap.h"

static uint8_t* heap_base = 0;
static uint8_t* heap_top = 0;
static uint8_t* heap_limit = 0;

void kheap_init(void* heap_start, size_t heap_size)
{
    heap_base = (uint8_t*)heap_start;
    heap_top = heap_base;
    heap_limit = heap_base + heap_size;
}

static void* bump_alloc(size_t size, size_t align)
{
    if (align == 0) align = 1;

    uintptr_t current = (uintptr_t)heap_top;
    uintptr_t aligned = (current + (align - 1)) & ~(align - 1);

    uint8_t* p = (uint8_t*)aligned;
    uint8_t* new_top = p + size;

    if (new_top > heap_limit) {
        return NULL;
    }

    heap_top = new_top;
    return p;
}

void* kmalloc(size_t size)
{
    if (!heap_base) return NULL;
    if (size == 0) return NULL;
    return bump_alloc(size, 8);
}

void* kcalloc(size_t nmemb, size_t size)
{
    size_t total;
    if (nmemb == 0 || size == 0) return NULL;
    if (nmemb > (size_t)-1 / size) return NULL;

    total = nmemb * size;
    uint8_t* p = (uint8_t*)kmalloc(total);
    if (!p) return NULL;

    for (size_t i = 0; i < total; i++)
    {
        p[i] = 0;
    }
}

void* krealloc(void* ptr, size_t new_size)
{
    // ganz einfache Variante:
    // - wenn ptr == NULL -> wie kmalloc
    // - wenn new_size == 0 -> „frei“ ignorieren, gibt NULL zurück
    // - ansonsten neuen Block allokieren und OHNE alte Datenkopie zurückgeben
    //   (vollkommen fake, aber stört nicht, solange du es bewusst nutzt)

    if (ptr == NULL) {
        return kmalloc(new_size);
    }
    if (new_size == 0) {
        return NULL;
    }

    // Für jetzt einfach neuen Block geben – keine echte Realloc-Logik
    return kmalloc(new_size);
}
