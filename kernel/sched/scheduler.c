#include <kernel/scheduler.h>
#include <kernel/kernel.h>
#include <kernel/smp.h>
#include <arch/x86/gdt.h>
#include <mm/heap.h>
#include <drivers/serial.h>
#include <drivers/pit.h>
#include <sync/spinlock.h>
#include <string.h>

#define BLACK_DEFAULT_STACK  8192
#define BLACK_MLFQ_TIMESLICE_BASE 5

static black_task_t black_tasks[BLACK_MAX_TASKS];
static uint32_t black_next_tid = 1;
static black_spinlock_t black_task_lock = SPINLOCK_INIT;

static black_task_t *black_current = NULL;
static black_task_t *black_idle = NULL;

static black_task_t *black_mlfq_heads[BLACK_MLFQ_LEVELS];
static black_task_t *black_mlfq_tails[BLACK_MLFQ_LEVELS];
static black_spinlock_t black_mlfq_lock = SPINLOCK_INIT;

static volatile int black_sched_enabled = 0;
static volatile int black_sched_lock = 0;

static void black_idle_func(void) { while (1) __asm__ volatile("hlt"); }

static void black_enqueue(black_task_t *black_t)
{
    uint32_t black_level = black_t->black_mlfq_level;
    if (black_level >= BLACK_MLFQ_LEVELS) black_level = BLACK_MLFQ_LEVELS - 1;
    black_t->black_next = NULL;
    black_t->black_state = BLACK_TASK_STATE_READY;

    uint32_t black_fl;
    black_spinlock_irq_save(&black_mlfq_lock, &black_fl);
    if (black_mlfq_tails[black_level]) {
        black_mlfq_tails[black_level]->black_next = black_t;
    } else {
        black_mlfq_heads[black_level] = black_t;
    }
    black_mlfq_tails[black_level] = black_t;
    black_spinlock_irq_restore(&black_mlfq_lock, black_fl);
}

static black_task_t *black_dequeue(void)
{
    uint32_t black_fl;
    black_spinlock_irq_save(&black_mlfq_lock, &black_fl);

    for (int black_i = 0; black_i < BLACK_MLFQ_LEVELS; black_i++) {
        if (black_mlfq_heads[black_i]) {
            black_task_t *black_t = black_mlfq_heads[black_i];
            black_mlfq_heads[black_i] = black_t->black_next;
            if (!black_mlfq_heads[black_i]) black_mlfq_tails[black_i] = NULL;
            black_t->black_next = NULL;
            black_spinlock_irq_restore(&black_mlfq_lock, black_fl);
            return black_t;
        }
    }

    black_spinlock_irq_restore(&black_mlfq_lock, black_fl);
    return NULL;
}

void black_scheduler_init(void)
{
    memset(black_tasks, 0, sizeof(black_tasks));
    memset(black_mlfq_heads, 0, sizeof(black_mlfq_heads));
    memset(black_mlfq_tails, 0, sizeof(black_mlfq_tails));

    black_task_t *black_kernel = &black_tasks[0];
    black_kernel->black_tid = 0;
    black_kernel->black_pid = 0;
    strcpy(black_kernel->black_name, "black_kernel");
    black_kernel->black_state = BLACK_TASK_STATE_RUNNING;
    black_kernel->black_priority = 0;
    black_kernel->black_mlfq_level = 0;
    black_kernel->black_time_slice = 100;
    black_kernel->black_kernel_stack = (uint32_t *)kmalloc(BLACK_DEFAULT_STACK);
    black_kernel->black_kernel_stack_size = BLACK_DEFAULT_STACK;
    black_kernel->black_cpu_affinity = CPU_AFFINITY_NONE;
    black_kernel->black_stack_canary = __stack_chk_guard;
    black_current = black_kernel;

    black_idle = black_task_create("black_idle", black_idle_func, BLACK_DEFAULT_STACK);
    if (black_idle) {
        black_idle->black_priority = 255;
        black_idle->black_mlfq_level = BLACK_MLFQ_LEVELS - 1;
    }
}

black_task_t *black_task_create(const char *black_name, void (*black_entry)(void), uint32_t black_stack_size)
{
    uint32_t black_fl;
    black_spinlock_irq_save(&black_task_lock, &black_fl);

    black_task_t *black_t = NULL;
    for (int black_i = 0; black_i < BLACK_MAX_TASKS; black_i++) {
        if (black_tasks[black_i].black_state == BLACK_TASK_STATE_UNUSED) {
            black_t = &black_tasks[black_i];
            black_t->black_state = BLACK_TASK_STATE_READY;
            break;
        }
    }
    black_spinlock_irq_restore(&black_task_lock, black_fl);

    if (!black_t) return NULL;
    if (black_stack_size == 0) black_stack_size = BLACK_DEFAULT_STACK;

    uint32_t *black_stack = (uint32_t *)kmalloc(black_stack_size);
    if (!black_stack) { black_t->black_state = BLACK_TASK_STATE_UNUSED; return NULL; }

    black_t->black_tid = __sync_fetch_and_add(&black_next_tid, 1);
    black_t->black_pid = black_current ? black_current->black_pid : 0;
    strncpy(black_t->black_name, black_name, 31);
    black_t->black_name[31] = '\0';
    black_t->black_priority = 10;
    black_t->black_base_priority = 10;
    black_t->black_mlfq_level = 0;
    black_t->black_time_slice = BLACK_MLFQ_TIMESLICE_BASE;
    black_t->black_time_used = 0;
    black_t->black_kernel_stack = black_stack;
    black_t->black_kernel_stack_size = black_stack_size;
    black_t->black_cpu_affinity = CPU_AFFINITY_NONE;
    black_t->black_last_cpu = 0;
    black_t->black_stack_canary = __stack_chk_guard ^ black_t->black_tid;

    uint32_t *black_sp = (uint32_t *)((uint32_t)black_stack + black_stack_size);
    *(--black_sp) = (uint32_t)black_task_exit;
    *(--black_sp) = (uint32_t)black_entry;
    *(--black_sp) = 0;
    *(--black_sp) = 0;
    *(--black_sp) = 0;
    *(--black_sp) = 0;
    black_t->black_esp = (uint32_t)black_sp;

    black_enqueue(black_t);
    return black_t;
}

black_task_t *black_task_current(void) { return black_current; }

void black_schedule(void)
{
    if (!black_sched_enabled || black_sched_lock) return;
    black_schedule_force();
}

void black_schedule_force(void)
{
    if (black_sched_lock) return;

    black_task_t *black_next = black_dequeue();
    if (!black_next) {
        if (black_idle && black_current != black_idle) {
            black_next = black_idle;
        } else return;
    }

    if (black_next == black_current) return;

    black_task_t *black_prev = black_current;
    if (black_prev && black_prev->black_state == BLACK_TASK_STATE_RUNNING && black_prev != black_idle) {
        if (black_prev->black_time_used >= black_prev->black_time_slice) {
            if (black_prev->black_mlfq_level < BLACK_MLFQ_LEVELS - 1)
                black_prev->black_mlfq_level++;
            black_prev->black_time_slice = BLACK_MLFQ_TIMESLICE_BASE * (1 << black_prev->black_mlfq_level);
            black_prev->black_time_used = 0;
        }
        black_enqueue(black_prev);
    }

    black_current = black_next;
    black_next->black_state = BLACK_TASK_STATE_RUNNING;

    if (black_next->black_kernel_stack) {
        uint32_t black_top = (uint32_t)black_next->black_kernel_stack + black_next->black_kernel_stack_size;
        black_gdt_set_kernel_stack(black_top);
    }

    if (black_prev)
        black_context_switch(&black_prev->black_esp, black_next->black_esp);
}

void black_scheduler_tick(void)
{
    if (!black_sched_enabled || !black_current) return;
    black_current->black_cpu_time++;
    black_current->black_time_used++;

    if (black_current->black_tid != 0 && black_current->black_time_used >= black_current->black_time_slice) {
        black_schedule();
    }
}

void black_task_block(uint8_t black_reason)
{
    if (!black_current) return;
    black_current->black_state = BLACK_TASK_STATE_BLOCKED;
    black_current->black_block_reason = black_reason;
    black_schedule_force();
}

void black_task_unblock(black_task_t *black_t)
{
    if (!black_t || black_t->black_state != BLACK_TASK_STATE_BLOCKED) return;
    black_t->black_block_reason = BLOCK_REASON_NONE;
    black_t->black_mlfq_level = 0;
    black_t->black_time_slice = BLACK_MLFQ_TIMESLICE_BASE;
    black_t->black_time_used = 0;
    black_enqueue(black_t);
}

void black_task_sleep(uint32_t black_ms)
{
    if (!black_current) return;
    black_current->black_wake_time = black_pit_get_uptime_ms() + black_ms;
    black_current->black_state = BLACK_TASK_STATE_SLEEPING;
    black_schedule();
}

void black_task_exit(int black_status)
{
    (void)black_status;
    if (!black_current) return;
    if (black_current->black_kernel_stack) kfree(black_current->black_kernel_stack);
    black_current->black_state = BLACK_TASK_STATE_ZOMBIE;
    black_schedule_force();
    while (1) black_hlt();
}

void black_scheduler_enable(void) { black_sched_enabled = 1; }
void black_scheduler_disable(void) { black_sched_enabled = 0; }

void black_task_set_priority(black_task_t *black_t, uint8_t black_p)
{
    if (!black_t) return;
    black_t->black_base_priority = black_p;
    black_t->black_priority = black_p;
}

void black_task_set_affinity(black_task_t *black_t, uint32_t black_cpu)
{
    if (black_t) black_t->black_cpu_affinity = black_cpu;
}

int black_scheduler_get_task_count(void)
{
    int black_count = 0;
    for (int black_i = 0; black_i < BLACK_MAX_TASKS; black_i++) {
        if (black_tasks[black_i].black_state != BLACK_TASK_STATE_UNUSED)
            black_count++;
    }
    return black_count;
}

black_task_t *black_scheduler_get_tasks(int *black_max)
{
    if (black_max) *black_max = BLACK_MAX_TASKS;
    return black_tasks;
}

void black_scheduler_work_steal(void) {}
void black_scheduler_enqueue_cpu(black_task_t *black_t, uint32_t black_cpu)
{
    (void)black_cpu;
    if (black_t) black_enqueue(black_t);
}
