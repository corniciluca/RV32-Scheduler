#ifndef SCHEDULER_H
#define SCHEDULER_H
#include "common.h"

#define MAX_TASKS 8
#define TIME_SLICE 100 // in milliseconds
#define MAX_PRIORITY 8
#define FRAME_SIZE 144

#define CLINT_BASE 0x2000000
#define CLINT_MTIME (CLINT_BASE + 0xBFF8)
#define CLINT_MTIMECMP (CLINT_BASE + 0x4000)

typedef enum {
    TASK_RUNNING,
    TASK_READY,
    TASK_BLOCKED,
    TASK_TERMINATED
} task_state_t;

typedef struct task_t {
    vaddr_t *sp;
    uint8_t stack[1024];
    task_state_t state;
    uint8_t priority;
    void (*task_func)(void*);
    void* args;
    struct task_t* next;
    uint8_t id;
    char name[32];
} task_t;

// --- Global Variables (Extern Declarations) ---

// Ready queues (one for each priority level)
extern task_t *ready_head[MAX_PRIORITY];
extern task_t *ready_tail[MAX_PRIORITY];

// Currently executing task
extern task_t* current_task;

// Array of all task structures
extern task_t tasks[MAX_TASKS];

// Number of active tasks
extern uint8_t task_count;

// Bitmap indicating non-empty ready queues
extern uint8_t bitmap_ready_queues;


// --- Assembly Function Prototypes (Extern Declarations) ---

extern void trap_entry(void);
extern void task_exit(void);
extern void switch_to_first_task(void);


// --- Internal Static/Inline Function Prototypes ---
// Inline functions are typically defined in the header file.

/**
 * @brief Reads the Machine Cause (mcause) CSR.
 * @return The value of mcause.
 */
static inline uint32_t r_mcause() {
    uint32_t x;
    __asm__ volatile("csrr %0, mcause" : "=r"(x));
    return x;
}

/**
 * @brief Writes a value to the Machine Interrupt Enable (mie) CSR.
 * @param x The value to write to mie.
 */
static inline void w_mie(uint32_t x) {
    (void)x; // Silence unused parameter warning
    __asm__ volatile("csrw mie, %0" : : "r"(x));
}

/**
 * @brief Reads the Machine Interrupt Enable (mie) CSR.
 * @return The value of mie.
 */
static inline uint32_t r_mie() {
    uint32_t x;
    __asm__ volatile("csrr %0, mie" : "=r"(x));
    return x;
}

/**
 * @brief Enables global interrupts (sets MIE bit in mstatus).
 */
static inline void enable_global_interrupts() {
    __asm__ volatile("csrsi mstatus, 8");
}

/**
 * @brief Disables global interrupts (clears MIE bit in mstatus).
 */
static inline void disable_global_interrupts() {
    __asm__ volatile("csrci mstatus, 8");
}


// --- Public API Function Prototypes ---

/**
 * @brief Initializes the scheduler's ready queues and task count.
 */
void scheduler_init(void);

/**
 * @brief Enables the machine timer interrupt in the MIE CSR.
 */
void enable_timer_interrupts(void);

/**
 * @brief Disables the machine timer interrupt in the MIE CSR.
 */
void disable_timer_interrupts(void);

/**
 * @brief Resets the timer and enables timer interrupts.
 */
void timer_init(void);

/**
 * @brief Starts the scheduler, selects the first task, initializes the timer, and switches context.
 */
void scheduler_start(void);

/**
 * @brief Creates a new task.
 * @param task_func The entry point function for the task.
 * @param args Arguments to pass to the task function.
 * @param priority The priority level of the task (0 to MAX_PRIORITY-1).
 * @param name A name for the task.
 * @return The task ID if successful, or -1 on failure (e.g., MAX_TASKS reached).
 */
int task_create(void (*task_func)(void*), void* args, uint8_t priority, const char* name);

/**
 * @brief Exits the current task, setting its state to TERMINATED, and calls the scheduler.
 */
// void task_exit(void); // Declared as extern assembly function above

/**
 * @brief Adds a task to the appropriate ready queue.
 * @param task The task structure to enqueue.
 */
void enqueue_task(task_t* task);

/**
 * @brief The main scheduler function, called to select the next task to run.
 */
void trap_handler(void);

void schedule(void);

/**
 * @brief Finds and dequeues the next highest-priority ready task.
 * @return A pointer to the next task to run, or NULL if no task is ready.
 */
task_t* next_task(void);

/**
 * @brief Performs the context switch between the current and next task.
 * @param next_task The task to switch to.
 */
void context_switch(task_t* next_task);

/**
 * @brief Resets the RISC-V timer to fire after TIME_SLICE milliseconds.
 */
void timer_reset(void);


#endif // SCHEDULER_H
