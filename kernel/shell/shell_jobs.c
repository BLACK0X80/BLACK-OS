#include <kernel/kernel.h>
#include <drivers/serial.h>

#define BLACK_MAX_JOBS 32

typedef struct {
    uint32_t black_pid;
    int black_running;
    char black_name[64];
} black_job_t;

static black_job_t black_jobs[BLACK_MAX_JOBS];
static int black_job_count = 0;

void black_shell_jobs_init(void) {
    black_job_count = 0;
}

int black_shell_jobs_add(uint32_t black_pid, const char *black_name) {
    if (black_job_count >= BLACK_MAX_JOBS) return -1;
    black_jobs[black_job_count].black_pid = black_pid;
    black_jobs[black_job_count].black_running = 1;
    int black_i = 0;
    while (black_name[black_i] && black_i < 63) { black_jobs[black_job_count].black_name[black_i] = black_name[black_i]; black_i++; }
    black_jobs[black_job_count].black_name[black_i] = '\0';
    return black_job_count++;
}

void black_shell_jobs_list(void) {
    for (int black_i = 0; black_i < black_job_count; black_i++) {
        if (black_jobs[black_i].black_running) {
            black_serial_puts("[JOB] ");
            black_serial_puts(black_jobs[black_i].black_name);
            black_serial_putchar('\n');
        }
    }
}
