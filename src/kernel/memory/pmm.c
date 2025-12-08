#include "kernel/memory/pmm.h"

#define FRAME_SIZE  4096

#define MAX_FRAMES  (128 * 1024 * 1024 / FRAME_SIZE)
#define BITMAP_SIZE (MAX_FRAMES / 32)

static uint32_t frame_bitmap[BITMAP_SIZE];
static uint32_t total_frames = 0;

static inline void  set_bit(uint32_t idx)       { frame_bitmap[idx / 32] |= (1u << (idx % 32)); }
static inline void  clear_bits(uint32_t idx)    { frame_bitmap[idx / 32] &= ~(1u << (idx % 32)); }
static inline int   test_bit(uint32_t idx)      { return (frame_bitmap[idx / 32] & (1u << (idx % 32))) != 0; }

void pmm_init(uint32_t mem_size_bytes)
{
    total_frames = mem_size_bytes / FRAME_SIZE;
    if (total_frames > MAX_FRAMES)
        total_frames = MAX_FRAMES;

    for (uint32_t i = 0; i < BITMAP_SIZE; i++)
        frame_bitmap[i] = 0xFFFFFFFFu;

    uint32_t reserved = 256;
    if (reserved > total_frames)
        reserved = total_frames;

    for (uint32_t i = reserved; i < total_frames; i++)
        clear_bits(i);
    
}

void* pmm_alloc_frame(void)
{
    for (uint32_t i = 0; i < total_frames; i++) {
        if (!test_bit(i))
        {
            set_bit(i);
            return (void*)(uintptr_t)(i * FRAME_SIZE);
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
    clear_bits(idx);
}