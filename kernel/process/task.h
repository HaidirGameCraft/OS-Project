#pragma once

/**
 * Multitasking
 * Created By: Haidir
 * Based On: https://wiki.osdev.org/Cooperative_Multitasking
 */

#include <stdint.h>
#include <cpu/isr.h>

#define TASK_FLAGS_KILL     0b00100000
#define MAX_TASK_TIMES      10

typedef struct task_t {
    u32 eip;
    u32 cr3;
    u32 eflags;

    u32 esp;
    u32 old_esp;
    u32 flags;
    struct task_t *next;
    struct task_t *prev;
} task_t;


/**
 * task_initialize()
 * - initialize the task component
 */
void task_initialize();

/**
 * create_task()
 * @param main - the address to run as backgrond task
 * @param stack - the address of stack ( must be started at bottom which mean [stack + <size of stack>])
 */
void create_task(void (*main)(), u32 stack);

/**
 * task_kill()
 * - must be added before the program/address end
 */
void task_kill();

/**
 * SwitchTask()
 * - switch the task when desired
 * - it would be remove the task_t* when the flags is TASK_FLAGS_KILL
 * - This method has been use on Time_Handle
 */
void SwitchTask();