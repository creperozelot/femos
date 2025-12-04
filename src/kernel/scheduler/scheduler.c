// src/kernel/scheduler/scheduler.c
#include <stdint.h>
#include "kernel/scheduler/scheduler.h"

#define MAX_TASKS   4
#define STACK_SIZE  4096

typedef struct task {
    uint32_t*    sp;
    int          id;
    struct task* next;
} task_t;

static task_t  tasks[MAX_TASKS];
static uint8_t task_stacks[MAX_TASKS][STACK_SIZE];

static int      task_count   = 0;
static task_t*  current_task = 0;

extern void context_switch(uint32_t** old_sp, uint32_t* new_sp);

void scheduler_init(void)
{
    task_count   = 0;
    current_task = 0;
}

int scheduler_create(void (*entry)(void))
{
    if (task_count >= MAX_TASKS) {
        return -1;
    }

    int     id = task_count;
    task_t* t  = &tasks[id];

    uint32_t* sp = (uint32_t*)(task_stacks[id] + STACK_SIZE);

    // Stack für neuen Task:
    // context_switch -> pop edi, esi, ebx, ebp, ret
    *--sp = (uint32_t)entry; // ret -> entry
    *--sp = 0;               // ebp
    *--sp = 0;               // ebx
    *--sp = 0;               // esi
    *--sp = 0;               // edi

    t->sp = sp;
    t->id = id;

    if (task_count == 0) {
        t->next = t;         // erster Task -> Ring auf sich selbst
    } else {
        t->next = tasks[0].next;
        tasks[0].next = t;
    }

    task_count++;
    return id;
}

void scheduler_yield(void)
{
    if (task_count < 2 || !current_task) {
        return;
    }

    __asm__ __volatile__("cli");  // während des Wechsels Sperren
    task_t* prev = current_task;
    task_t* next = current_task->next;
    current_task = next;

    context_switch(&prev->sp, next->sp);

    __asm__ __volatile__("sti");  // WICHTIG: nach dem Switch wieder an!
}

void scheduler_start(void)
{
    if (task_count == 0) return;

    current_task = &tasks[0];

    uint32_t* dummy = 0;
    context_switch(&dummy, current_task->sp);

    for (;;) __asm__ __volatile__("hlt");
}
