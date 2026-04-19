#include <kernel/smp.h>
#include <kernel/kernel.h>
#include <drivers/serial.h>
#include <string.h>

black_per_cpu_t black_cpu_data[BLACK_MAX_CPUS];
volatile uint32_t black_cpu_count = 1;
volatile uint32_t black_cpu_online_count = 1;

void black_smp_init(void)
{
    memset(black_cpu_data, 0, sizeof(black_cpu_data));
    black_cpu_data[0].black_cpu_id = 0;
    black_cpu_data[0].black_lapic_id = 0;
    black_cpu_data[0].black_state = CPU_STATE_ONLINE;
    black_cpu_data[0].black_run_queue_head = NULL;
    black_cpu_data[0].black_run_queue_tail = NULL;
    black_cpu_data[0].black_run_queue_len = 0;
}

void black_smp_start_aps(void) {}

uint32_t black_smp_get_cpu_id(void) { return 0; }
uint32_t black_smp_get_cpu_count(void) { return black_cpu_count; }
uint32_t black_smp_get_online_count(void) { return black_cpu_online_count; }
int  black_smp_is_bsp(void) { return 1; }
void black_smp_scheduler_tick(void) {}
