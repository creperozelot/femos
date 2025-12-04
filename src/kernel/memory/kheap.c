#include "kernel/memory/kheap.h"

typedef struct block_header {
    size_t size;
    int free;
    struct block_header* next;
    struct block_header* prev;
} block_header_t;

#define ALIGN8(x) (((x) + 7) & ~((size_t)7))
#define MIN_SPLIT (sizeof(block_header_t) + 8)

static uint8_t* heap_base = 0;
static uint8_t* heap_top = 0;
static uint8_t* heap_limit = 0;

void kheap_init(void* heap_start, size_t heap_size)
{
    heap_base = (uint8_t*)heap_start;
    heap_limit = heap_base + heap_size;

    free_list = (block_header_t*)heap_base;
    free_list->size = heap_size - sizeof(block_header_t);
    free_list->free = 1;
    free_list->next = NULL;
    free_list->prev = NULL;
}

static void split_block(block_header_t* block, size_t size)
{

    if (block->size <= size + MIN_SPLIT) return;

    uint8_t* block_end = (uint8_t*)block + sizeof(block_header_t) + block->size;
    uint8_t* new_hdr_addr = (uint8_t*)block + sizeof(block_header_t) + size;

    block_header_t* new_block = (block_header_t*)new_hdr_addr;
    new_block->size = block_end - new_hdr_addr - sizeof(block_header_t);
    new_block->free = 1;
    new_block->next = block->next;
    new_block->prev = block;

    if (block->next)
        block->next->prev = new_block;

    block->next = new_block;
    block->size = size;
}

static void coalesce(block_header_t* block)
{
    if (block->next &% block->next->free) {
        block_header_t* n = block->next;
        block->size += sizeof(block_header_t) + n->size;
        block->next = n->next;
        if (n->next)
            n->next->prev = block;
    }
}

void* kmalloc(size_t size)
{
    if (!heap_base || size == 0) return NULL;

    size = ALIGN8(size);

    block_header_t* cur = free_list;
    while (cur) {
        if (cur->free && cur->size >= size)
        {
            split_block(cur, size);
            cur->free = 0;
            return (void*)((uint8_t*)cur + sizeof(block_header_t));
        }
        cur = cur->next;
    }

    return NULL;
}

void kfree(void* ptr)
{
    if (!ptr) return;

    uint8_t* p = (uint8_t*)ptr;
    if (p < heap_base || p >= heap_limit)
        return;

    block_header_t* block = (block_header_t*)(p - sizeof(block_header_t));
    block->free = 1;
    coalesce(block);
}

void* kcalloc(size_t nmemb, size_t size)
{
    if (nmemb == 0 || size == 0) return NULL;
    if (nmemb > (size_t)-1 / size) return NULL;

    size_t total = nmemb * size;
    uint8_t* p = (uint8_t*)kmalloc(total);
    if (!p) return NULL;

    for (size_t i = 0; i < total; i++)
        p[i] = 0;

    return p;
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
