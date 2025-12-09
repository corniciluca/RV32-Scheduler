#include "scheduler.h"
#include "common.h"

void task1(void* args){
    while(1){
        uart_wr("Task 1 running\n");
    }
}

void task2(void* args){
    while(1){
        uart_wr("Task 2 running\n");
    }
}

void task3(void* args){
    while(1){
        uart_wr("Task 3 running\n");
    }
}

int main(void){
    uart_wr("Scheduler starting...\n");

    scheduler_init();

    task_create(task2, NULL, 1, "task2");
    task_create(task3, NULL, 1, "task3");

    uart_wr("Tasks created. Starting scheduler...\n");
    scheduler_start();

    return 0;
}