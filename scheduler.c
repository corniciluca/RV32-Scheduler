#include "scheduler.h"

// --- Global variable definitions (matching externs in scheduler.h) ---
task_t *ready_head[MAX_PRIORITY];
task_t *ready_tail[MAX_PRIORITY];
task_t *current_task = NULL;
task_t tasks[MAX_TASKS];
uint8_t task_count = 0;

uint8_t bitmap_ready_queues = 0;

void scheduler_init(void){
    for(int i = 0; i < MAX_PRIORITY; i++){
        ready_head[i] = NULL;
        ready_tail[i] = NULL;
    }
    current_task = NULL;

    for (int i = 0; i < MAX_TASKS; i++) {
        tasks[i].state = TASK_TERMINATED;
        tasks[i].next = NULL;
        tasks[i].id = i;
    }
    task_count = 0;
}

void enable_timer_interrupts(void){
    uart_wr("[timer] enable_timer_interrupts\n");
    uint32_t mie = r_mie();
    mie |= (1 << 7); // set MTIE
    w_mie(mie);
    uart_wr("[timer] MTIE set\n");
}

void disable_timer_interrupts(void){
    uint32_t mie = r_mie();
    mie &= ~(1 << 7);
    w_mie(mie);
}

void timer_init(void){
    uart_wr("[timer] timer_init\n");
    timer_reset();
    enable_timer_interrupts();
}
void scheduler_start(void){
    if (task_count == 0) return;
    
    current_task = next_task();

    timer_init();
    enable_global_interrupts();

    switch_to_first_task();
}

int task_create(void (*task_func)(void*), void* args, uint8_t priority, const char* name){
    int free_slot_id = -1;
    for(int i = 0; i < MAX_TASKS; i++){
        if(tasks[i].state == TASK_TERMINATED){
            free_slot_id = i;
            break;
        }
    }
    if(free_slot_id == -1) return -1;
    task_t* new_task = &tasks[free_slot_id];
    task_count++;
    new_task->id = free_slot_id;
    new_task->next = NULL;
    new_task->state = TASK_READY;
    new_task->priority = priority;
    new_task->task_func = task_func;
    new_task->args = args;
    strcpy(new_task->name, name);
    uint8_t* sp = new_task->stack+ sizeof(new_task->stack);
    sp -= FRAME_SIZE;  
    new_task->sp = (vaddr_t*)sp;

    uint32_t *stack_frame = (uint32_t *)sp;
    stack_frame[1] = (uint32_t)task_exit;  // ra
    stack_frame[3] = 0;                 // gp
    stack_frame[10] = (uint32_t)args;      // a0
    stack_frame[32] = (uint32_t)task_func; // mepc
    stack_frame[33] = 0x1880;              

    enqueue_task(new_task);

    bitmap_ready_queues |= (1 << priority);

    return free_slot_id;
}

void task_exit(void){
    current_task->state = TASK_TERMINATED;
    schedule();
}

void enqueue_task(task_t* task){
    uint8_t prio = task->priority;
    task->state = TASK_READY;
    task->next = (task_t*) NULL;
    if(ready_tail[prio] == (task_t*) NULL){
        ready_head[prio] = task;
        ready_tail[prio] = task;

        bitmap_ready_queues |= (1 << prio);
    } else {
        ready_tail[prio]->next = task;
        ready_tail[prio] = task;
    }
}

void trap_handler(void){
    uint32_t mcause = r_mcause();
    if((mcause & 0x80000000) && (mcause & 0xFF) == 7){
        uart_wr("[sched] timer interrupt\n");
        timer_reset();
    }
    schedule();
}

void schedule(void){
    uart_wr("[sched] schedule() called\n");
    if(current_task && current_task->state == TASK_RUNNING){
        enqueue_task(current_task);
    }

    task_t* next = next_task();
    context_switch(next);
}

task_t* next_task(void){
    if(bitmap_ready_queues == 0) return NULL;

    int prio = 31 - __builtin_clz(bitmap_ready_queues);
    
    task_t* next = ready_head[prio];
    ready_head[prio] = next->next;

    if(ready_head[prio] == NULL){
        ready_tail[prio] = NULL;
        bitmap_ready_queues &= ~(1 << prio);
    }

    next->next = NULL;
    next->state = TASK_RUNNING;
    return next;
}

void context_switch(task_t* next_task){
    task_t* prev_task = current_task;
    current_task = next_task;
}

void timer_reset(void){
    uint64_t *mtime = (uint64_t *)CLINT_MTIME;
    uint64_t *mtimecmp = (uint64_t *)CLINT_MTIMECMP;
    *mtimecmp = *mtime + TIME_SLICE * 1000;
}