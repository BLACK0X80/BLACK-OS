#include <kernel/process.h>
#include <kernel/kernel.h>
#include <kernel/scheduler.h>
#include <drivers/pit.h>
#include <string.h>

static black_process_t black_procs[MAX_PROCESSES];
static uint32_t black_current_pid = 0;
static uint32_t black_next_pid = 1;

void black_process_init(void)
{
    memset(black_procs, 0, sizeof(black_procs));
    black_procs[0].black_pid = 0;
    black_procs[0].black_ppid = 0;
    black_procs[0].black_state = PROC_STATE_RUNNING;
    strcpy(black_procs[0].black_name, "black_init");
    black_procs[0].black_start_time = (uint32_t)black_pit_get_uptime_ms();
    black_current_pid = 0;
    for (int black_i = 0; black_i < NSIG; black_i++)
        black_procs[0].black_signal_handlers[black_i] = NULL;
}

uint32_t black_process_create(const char *black_name, uint32_t black_ppid)
{
    for (int black_i = 0; black_i < MAX_PROCESSES; black_i++) {
        if (black_procs[black_i].black_state == PROC_STATE_UNUSED) {
            memset(&black_procs[black_i], 0, sizeof(black_process_t));
            black_procs[black_i].black_pid = black_next_pid++;
            black_procs[black_i].black_ppid = black_ppid;
            black_procs[black_i].black_state = PROC_STATE_READY;
            strncpy(black_procs[black_i].black_name, black_name, PROC_NAME_LEN - 1);
            black_procs[black_i].black_start_time = (uint32_t)black_pit_get_uptime_ms();
            black_procs[black_i].black_priority = 10;
            return black_procs[black_i].black_pid;
        }
    }
    return 0;
}

int black_process_kill(uint32_t black_pid)
{
    for (int black_i = 0; black_i < MAX_PROCESSES; black_i++) {
        if (black_procs[black_i].black_pid == black_pid && black_procs[black_i].black_state != PROC_STATE_UNUSED) {
            black_procs[black_i].black_state = PROC_STATE_ZOMBIE;
            black_process_reparent_children(black_pid);
            return 0;
        }
    }
    return -1;
}

black_process_t *black_process_get(uint32_t black_pid)
{
    for (int black_i = 0; black_i < MAX_PROCESSES; black_i++) {
        if (black_procs[black_i].black_pid == black_pid && black_procs[black_i].black_state != PROC_STATE_UNUSED)
            return &black_procs[black_i];
    }
    return NULL;
}

black_process_t *black_process_current(void) { return &black_procs[0]; }
void black_process_set_current(uint32_t black_pid) { black_current_pid = black_pid; }
const char *black_process_state_name(uint8_t black_s)
{
    static const char *black_names[] = { "unused", "running", "ready", "blocked", "zombie", "stopped" };
    return black_s <= 5 ? black_names[black_s] : "unknown";
}

uint32_t black_process_count(void)
{
    uint32_t black_c = 0;
    for (int black_i = 0; black_i < MAX_PROCESSES; black_i++)
        if (black_procs[black_i].black_state != PROC_STATE_UNUSED) black_c++;
    return black_c;
}

black_process_t *black_process_iterate(uint32_t *black_idx)
{
    while (*black_idx < MAX_PROCESSES) {
        if (black_procs[*black_idx].black_state != PROC_STATE_UNUSED) return &black_procs[(*black_idx)++];
        (*black_idx)++;
    }
    return NULL;
}

void black_process_reparent_children(uint32_t black_ppid)
{
    for (int black_i = 0; black_i < MAX_PROCESSES; black_i++) {
        if (black_procs[black_i].black_ppid == black_ppid && black_procs[black_i].black_state != PROC_STATE_UNUSED)
            black_procs[black_i].black_ppid = 0;
    }
}

int32_t black_process_signal(uint32_t black_pid, int black_sig)
{
    black_process_t *black_p = black_process_get(black_pid);
    if (!black_p || black_sig < 0 || black_sig >= NSIG) return -1;
    if (black_sig == SIGKILL) { black_process_kill(black_pid); return 0; }
    black_p->black_pending_signals |= (1 << black_sig);
    return 0;
}

void black_process_check_signals(void)
{
    black_process_t *black_p = black_process_current();
    if (!black_p) return;
    for (int black_s = 0; black_s < NSIG; black_s++) {
        if (black_p->black_pending_signals & (1 << black_s)) {
            black_p->black_pending_signals &= ~(1 << black_s);
            if (black_p->black_signal_handlers[black_s])
                black_p->black_signal_handlers[black_s](black_s);
            else if (black_s == SIGTERM)
                black_process_kill(black_p->black_pid);
        }
    }
}

int32_t black_process_fork(void) { return -1; }
int32_t black_process_exec(const char *black_path, char *const black_argv[]) { (void)black_path; (void)black_argv; return -1; }
int32_t black_process_waitpid(int32_t black_pid, int32_t *black_status, int black_opts) { (void)black_pid; (void)black_status; (void)black_opts; return -1; }
void    black_process_exit(int32_t black_status) { (void)black_status; }
