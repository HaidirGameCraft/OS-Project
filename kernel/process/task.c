#include <process/task.h>
#include <memory.h>
#include <string.h>
#include <stdio.h>

static task_t main_task;
static task_t* current = NULL;
u32 islock = 0;

extern void TaskInitialize(task_t* main);
extern void __switch_task__(task_t* current, task_t* next);
extern void __switch_task_single__(task_t* next);
u8 stack_switch[1024];

void task_initialize() {
    TaskInitialize( &main_task );

    main_task.next = &main_task;
    main_task.prev = NULL;
    current = &main_task;
}

void create_task(void (*main)(), u32 stack) {
    // allocate new task_t
    task_t* new_task = (task_t*) malloc( sizeof( task_t ) );
    new_task->esp = (u32) stack;
    new_task->old_esp = stack;
    new_task->flags = 0;
    new_task->eip = (u32) main;

    // Initialize the register, flags and page_directory on stack bottom
    u32* esp = (u32*) new_task->esp;
    ((u32*) --esp)[0] = (u32) main;           // Return Value Address
    ((u32*) --esp)[0] = 0x00;                 // EAX
    ((u32*) --esp)[0] = 0x00;                 // ECX
    ((u32*) --esp)[0] = 0x00;                 // EDX
    ((u32*) --esp)[0] = 0x00;                 // EBX
    ((u32*) --esp)[0] = new_task->esp;        // Old ESP
    ((u32*) --esp)[0] = 0x00;                 // EBP
    ((u32*) --esp)[0] = 0x00;                 // ESI
    ((u32*) --esp)[0] = 0x00;                 // EDI
    ((u32*) --esp)[0] = main_task.cr3;        // CR3
    ((u32*) --esp)[0] = 0x00;                 // EFLAGS
    new_task->esp = (u32) esp;

    task_t* crt = &main_task;
    // find the matching of crt->next is the main_task
    while( 1 )
    {
        if( crt->next == &main_task )
        {
            new_task->next = crt->next;
            crt->next = new_task;
            new_task->prev = crt;
            
            new_task->cr3 = main_task.cr3;
            break;
        }
        
        crt = crt->next;
    }
}

void task_kill() {
    if( current == &main_task )
        return;       // DO NOT KILL THE MAIN TASK

    current->flags |= TASK_FLAGS_KILL;
    //printf("Task (%x) has been killed. move to %x\n", (u32) current, (u32) current->next);
    SwitchTask();
}

void SwitchTask() {
    task_t* c = (task_t*) current;
    current = c->next;

    //printf("Switch Task (%x) -> (%x): FLAGS: %x \n", c, c->next, c->flags);

    extern u32 task_time;
    // Analysis and remove the task that has been killed [flags = TASK_FLAGS_KILL]
    while( current->flags && TASK_FLAGS_KILL )
    {
        task_t* tkill = current;
        current = tkill->next;
        c->next = current;
        free( (void*) tkill->old_esp );
        free( (void*) tkill );
        task_time = 0;
    }
    task_time = 0;
    __switch_task__( c, current );
    //switch_task(current, current->next);
}