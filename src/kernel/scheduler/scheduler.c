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

// wird von context_switch.asm bereitgestellt
extern void context_switch(uint32_t** old_sp, uint32_t* new_sp);

// Timer-Signal: „Es wäre Zeit für einen Taskwechsel“
static volatile int need_resched = 0;

void scheduler_tick(void)
{
    // Wird im PIT-IRQ-Kontext aufgerufen.
    // Hier NICHT context_switch aufrufen, sondern nur ein Flag setzen.
    need_resched = 1;
}

void scheduler_init(void)
{
    task_count   = 0;
    current_task = 0;
    need_resched = 0;
}

int scheduler_create(void (*entry)(void))
{
    if (task_count >= MAX_TASKS) {
        return -1;
    }

    int     id = task_count;
    task_t* t  = &tasks[id];

    // eigener Stack für Task: oben beginnen
    uint32_t* sp = (uint32_t*)(task_stacks[id] + STACK_SIZE);

    // Stack-Layout passend zu context_switch:
    // context_switch:
    //   push ebp, ebx, esi, edi
    //   *old_sp = esp
    //   esp = new_sp
    //   pop edi, esi, ebx, ebp
    //   ret  -> springt zu entry
    //
    // Stack (oben -> unten):
    //   [edi][esi][ebx][ebp][ret_eip]

    *--sp = (uint32_t)entry; // ret -> entry
    *--sp = 0;               // ebp
    *--sp = 0;               // ebx
    *--sp = 0;               // esi
    *--sp = 0;               // edi

    t->sp = sp;
    t->id = id;

    if (task_count == 0) {
        t->next = t; // erster Task zeigt auf sich selbst
    } else {
        t->next = tasks[0].next;
        tasks[0].next = t;
    }

    task_count++;
    return id;
}

void scheduler_yield(void)
{
    // kein Wechsel nötig oder nur ein Task
    if (!need_resched || task_count < 2 || !current_task) {
        return;
    }

    need_resched = 0;

    task_t* prev = current_task;
    task_t* next = current_task->next;
    current_task = next;

    // WICHTIG: KEIN cli/sti hier!
    // Beim ersten Wechsel kehrt context_switch NICHT in scheduler_yield zurück,
    // sondern direkt in entry() des neuen Tasks.
    // Ein sti nach context_switch würde daher nie ausgeführt werden.
    context_switch(&prev->sp, next->sp);
}

void scheduler_start(void)
{
    if (task_count == 0) {
        return;
    }

    current_task = &tasks[0];

    uint32_t* dummy = 0;
    context_switch(&dummy, current_task->sp);

    // sollte nie erreicht werden
    for (;;) {
        __asm__ __volatile__("hlt");
    }
}
