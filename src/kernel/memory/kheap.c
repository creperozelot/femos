// src/kernel/memory/kheap.c
#include "kernel/memory/kheap.h"
#include "kernel/memory/pmm.h"

typedef struct block_header {
    size_t size;
    int    free;
    struct block_header* next;
    struct block_header* prev;
} block_header_t;

#define ALIGN8(x)   (((x) + 7) & ~((size_t)7))
#define MIN_SPLIT   (sizeof(block_header_t) + 8)

static block_header_t* free_list = 0;

static void add_page_to_heap(void)
{
    void* frame = pmm_alloc_frame();
    if (!frame) {
        return;
    }

    block_header_t* block = (block_header_t*)frame;
    block->size = 4096 - sizeof(block_header_t);
    block->free = 1;

    block->prev = NULL;
    block->next = free_list;
    if (free_list)
        free_list->prev = block;
    free_list = block;
}

void kheap_init(void)
{
    free_list = NULL;

    // 2 Seiten initial als Heap holen
    add_page_to_heap();
    add_page_to_heap();
}

static void split_block(block_header_t* block, size_t size)
{
    if (block->size <= size + MIN_SPLIT)
        return;

    uint8_t* block_end    = (uint8_t*)block + sizeof(block_header_t) + block->size;
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
    if (block->next && block->next->free) {
        block_header_t* n = block->next;
        block->size += sizeof(block_header_t) + n->size;
        block->next = n->next;
        if (n->next)
            n->next->prev = block;
    }

    if (block->prev && block->prev->free) {
        block_header_t* p = block->prev;
        p->size += sizeof(block_header_t) + block->size;
        p->next = block->next;
        if (block->next)
            block->next->prev = p;
        block = p;
    }
}

void* kmalloc(size_t size)
{
    if (size == 0)
        return NULL;

    size = ALIGN8(size);

    for (;;) {
        block_header_t* cur = free_list;
        while (cur) {
            if (cur->free && cur->size >= size) {
                split_block(cur, size);
                cur->free = 0;
                return (void*)((uint8_t*)cur + sizeof(block_header_t));
            }
            cur = cur->next;
        }

        // keine passende Lücke – neue Seite
        add_page_to_heap();

        // wenn add_page_to_heap nichts gebracht hat -> Ende
        if (!free_list)
            return NULL;
    }
}

void kfree(void* ptr)
{
    if (!ptr)
        return;

    block_header_t* block = (block_header_t*)((uint8_t*)ptr - sizeof(block_header_t));
    block->free = 1;
    coalesce(block);
}

void* kcalloc(size_t nmemb, size_t size)
{
    if (nmemb == 0 || size == 0)
        return NULL;

    if (nmemb > (size_t)-1 / size)
        return NULL;

    size_t total = nmemb * size;
    uint8_t* p = (uint8_t*)kmalloc(total);
    if (!p) return NULL;

    for (size_t i = 0; i < total; i++)
        p[i] = 0;

    return p;
}

void* krealloc(void* ptr, size_t new_size)
{
    if (!ptr)
        return kmalloc(new_size);
    if (new_size == 0) {
        kfree(ptr);
        return NULL;
    }

    block_header_t* block = (block_header_t*)((uint8_t*)ptr - sizeof(block_header_t));
    size_t copy_size = block->size < new_size ? block->size : new_size;

    uint8_t* new_ptr = (uint8_t*)kmalloc(new_size);
    if (!new_ptr) return NULL;

    uint8_t* src = (uint8_t*)ptr;
    for (size_t i = 0; i < copy_size; i++)
        new_ptr[i] = src[i];

    kfree(ptr);
    return new_ptr;
}
