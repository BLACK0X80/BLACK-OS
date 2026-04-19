#ifndef _BLACK_SMP_H
#define _BLACK_SMP_H

#include <stdint.h>
#include <sync/spinlock.h>
#include <kernel/scheduler.h>

#define BLACK_MAX_CPUS        16
#define CPU_STATE_OFFLINE     0
#define CPU_STATE_ONLINE      1

typedef struct {
    uint32_t       black_cpu_id;
    uint32_t       black_lapic_id;
    uint32_t       black_state;
    black_task_t  *black_run_queue_head;
    black_task_t  *black_run_queue_tail;
    uint32_t       black_run_queue_len;
    black_spinlock_t black_run_queue_lock;
    uint32_t       black_kernel_stack;
} black_per_cpu_t;

typedef black_per_cpu_t per_cpu_t;

extern black_per_cpu_t black_cpu_data[BLACK_MAX_CPUS];
extern volatile uint32_t black_cpu_count;
extern volatile uint32_t black_cpu_online_count;

#define cpu_data         black_cpu_data
#define cpu_count        black_cpu_count
#define cpu_online_count black_cpu_online_count

void black_smp_init(void);
void black_smp_start_aps(void);
uint32_t black_smp_get_cpu_id(void);
uint32_t black_smp_get_cpu_count(void);
uint32_t black_smp_get_online_count(void);
int  black_smp_is_bsp(void);
void black_smp_scheduler_tick(void);

#define smp_init           black_smp_init
#define smp_start_aps      black_smp_start_aps
#define smp_get_cpu_id     black_smp_get_cpu_id
#define smp_get_cpu_count  black_smp_get_cpu_count
#define smp_get_online_count black_smp_get_online_count
#define smp_is_bsp         black_smp_is_bsp
#define smp_scheduler_tick black_smp_scheduler_tick

#endif
