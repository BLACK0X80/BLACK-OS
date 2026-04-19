#ifndef _BLACK_SCHEDULER_H
#define _BLACK_SCHEDULER_H

#include <stdint.h>

#define BLACK_TASK_STATE_UNUSED    0
#define BLACK_TASK_STATE_RUNNING   1
#define BLACK_TASK_STATE_READY     2
#define BLACK_TASK_STATE_BLOCKED   3
#define BLACK_TASK_STATE_SLEEPING  4
#define BLACK_TASK_STATE_ZOMBIE    5

#define TASK_STATE_UNUSED   BLACK_TASK_STATE_UNUSED
#define TASK_STATE_RUNNING  BLACK_TASK_STATE_RUNNING
#define TASK_STATE_READY    BLACK_TASK_STATE_READY
#define TASK_STATE_BLOCKED  BLACK_TASK_STATE_BLOCKED
#define TASK_STATE_SLEEPING BLACK_TASK_STATE_SLEEPING
#define TASK_STATE_ZOMBIE   BLACK_TASK_STATE_ZOMBIE

#define BLACK_MLFQ_LEVELS     4
#define BLACK_MAX_TASKS       128
#define CPU_AFFINITY_NONE     0xFFFFFFFF

#define BLOCK_REASON_NONE     0
#define BLOCK_REASON_IO       1
#define BLOCK_REASON_MUTEX    2
#define BLOCK_REASON_WAIT     3

typedef struct black_task {
    uint32_t black_tid;
    uint32_t black_pid;
    char     black_name[32];
    uint8_t  black_state;
    uint8_t  black_priority;
    uint8_t  black_base_priority;
    uint8_t  black_inherited_priority;
    uint8_t  black_mlfq_level;
    uint32_t black_time_slice;
    uint32_t black_time_used;
    uint32_t *black_kernel_stack;
    uint32_t black_kernel_stack_size;
    uint32_t black_esp;
    uint32_t *black_page_directory;
    uint64_t black_cpu_time;
    uint64_t black_wake_time;
    uint32_t black_cpu_affinity;
    uint32_t black_last_cpu;
    uint32_t black_stack_canary;
    uint8_t  black_block_reason;
    void    *black_waiting_on;
    struct black_task *black_next;
} black_task_t;

typedef black_task_t task_t;

void black_scheduler_init(void);
void black_scheduler_enable(void);
void black_scheduler_disable(void);
void black_schedule(void);
void black_schedule_force(void);
void black_scheduler_tick(void);
void black_scheduler_work_steal(void);

black_task_t *black_task_create(const char *black_name, void (*black_entry)(void), uint32_t black_stack_size);
black_task_t *black_task_current(void);
void black_task_exit(int black_status);
void black_task_block(uint8_t black_reason);
void black_task_unblock(black_task_t *black_task);
void black_task_sleep(uint32_t black_ms);
void black_task_set_priority(black_task_t *black_task, uint8_t black_priority);
void black_task_set_affinity(black_task_t *black_task, uint32_t black_cpu_id);
int  black_scheduler_get_task_count(void);
black_task_t *black_scheduler_get_tasks(int *black_max);
void black_scheduler_enqueue_cpu(black_task_t *black_task, uint32_t black_cpu);

extern void black_context_switch(uint32_t *black_old_esp, uint32_t black_new_esp);

#define scheduler_init        black_scheduler_init
#define scheduler_enable      black_scheduler_enable
#define scheduler_disable     black_scheduler_disable
#define schedule              black_schedule
#define schedule_force        black_schedule_force
#define scheduler_tick        black_scheduler_tick
#define task_create           black_task_create
#define task_current          black_task_current
#define task_exit             black_task_exit
#define task_block            black_task_block
#define task_unblock          black_task_unblock
#define task_sleep            black_task_sleep
#define context_switch        black_context_switch
#define scheduler_get_task_count black_scheduler_get_task_count
#define scheduler_get_tasks   black_scheduler_get_tasks
#define scheduler_work_steal  black_scheduler_work_steal

#endif
