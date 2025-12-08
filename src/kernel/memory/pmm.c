// src/kernel/memory/pmm.c
#include "kernel/memory/pmm.h"

#define FRAME_SIZE   4096
#define MAX_FRAMES   (128 * 1024 * 1024 / FRAME_SIZE)
#define BITMAP_SIZE  (MAX_FRAMES / 32)

static uint32_t frame_bitmap[BITMAP_SIZE];
static uint32_t total_frames = 0;

static inline void set_bit(uint32_t idx)   { frame_bitmap[idx / 32] |=  (1u << (idx % 32)); }
static inline void clear_bit(uint32_t idx) { frame_bitmap[idx / 32] &= ~(1u << (idx % 32)); }
static inline int  test_bit(uint32_t idx)  { return (frame_bitmap[idx / 32] & (1u << (idx % 32))) != 0; }

void pmm_init(uint32_t mem_size_bytes, uintptr_t kernel_end_phys)
{
    total_frames = mem_size_bytes / FRAME_SIZE;
    if (total_frames > MAX_FRAMES)
        total_frames = MAX_FRAMES;

    // erstmal alles als benutzt markieren
    for (uint32_t i = 0; i < BITMAP_SIZE; i++)
        frame_bitmap[i] = 0xFFFFFFFFu;

    // alle Frames ab kernel_end_phys freigeben
    uint32_t first_free_frame = (uint32_t)((kernel_end_phys + FRAME_SIZE - 1) / FRAME_SIZE);

    if (first_free_frame > total_frames)
        first_free_frame = total_frames;

    for (uint32_t i = first_free_frame; i < total_frames; i++)
        clear_bit(i);
}

void* pmm_alloc_frame(void)
{
    for (uint32_t i = 0; i < total_frames; i++) {
        if (!test_bit(i)) {
            set_bit(i);
            return (void*)(uintptr_t)(i * FRAME_SIZE); // Identity-Mapping
        }
    }
    return NULL;
}

void pmm_free_frame(void* addr)
{
    uintptr_t a = (uintptr_t)addr;
    uint32_t idx = a / FRAME_SIZE;
    if (idx >= total_frames)
        return;
    clear_bit(idx);
}
