#ifndef _BLACK_PROCESS_H
#define _BLACK_PROCESS_H

#include <stdint.h>

#define MAX_PROCESSES      128
#define MAX_FDS_PER_PROC   32
#define PROC_NAME_LEN      32
#define NSIG               32

#define PROC_STATE_UNUSED   0
#define PROC_STATE_RUNNING  1
#define PROC_STATE_READY    2
#define PROC_STATE_BLOCKED  3
#define PROC_STATE_ZOMBIE   4
#define PROC_STATE_STOPPED  5

#define SIGKILL  9
#define SIGTERM  15
#define SIGCHLD  17
#define SIGCONT  18
#define SIGSTOP  19
#define SIGTSTP  20
#define SIGTTIN  21
#define SIGTTOU  22
#define SIGURG   23
#define SIGWINCH 28

typedef void (*black_sighandler_t)(int);
typedef black_sighandler_t sighandler_t;

typedef struct {
    struct black_vfs_node *black_node;
    uint32_t black_offset;
    uint32_t black_flags;
    int      black_in_use;
    int      black_pipe_id;
} black_fd_entry_t;

typedef struct {
    uint32_t black_pid;
    uint32_t black_ppid;
    uint32_t black_pgid;
    uint8_t  black_state;
    char     black_name[PROC_NAME_LEN];
    uint32_t black_start_time;
    uint64_t black_cpu_time;
    uint8_t  black_priority;
    int32_t  black_exit_code;
    uint32_t black_waiting_for_pid;
    black_fd_entry_t black_fd_table[MAX_FDS_PER_PROC];
    uint32_t black_pending_signals;
    uint32_t black_blocked_signals;
    black_sighandler_t black_signal_handlers[NSIG];
    void    *black_elf_proc;
    uint32_t black_kernel_stack;
} black_process_t;

typedef black_process_t process_t;

void black_process_init(void);
uint32_t black_process_create(const char *black_name, uint32_t black_ppid);
int  black_process_kill(uint32_t black_pid);
black_process_t *black_process_get(uint32_t black_pid);
black_process_t *black_process_current(void);
void black_process_set_current(uint32_t black_pid);
uint32_t black_process_count(void);
black_process_t *black_process_iterate(uint32_t *black_idx);
const char *black_process_state_name(uint8_t black_state);
int32_t black_process_fork(void);
int32_t black_process_exec(const char *black_path, char *const black_argv[]);
int32_t black_process_waitpid(int32_t black_pid, int32_t *black_status, int black_opts);
void    black_process_exit(int32_t black_status);
int32_t black_process_signal(uint32_t black_pid, int black_sig);
void    black_process_check_signals(void);
void    black_process_reparent_children(uint32_t black_ppid);

#define process_init              black_process_init
#define process_create            black_process_create
#define process_kill              black_process_kill
#define process_get               black_process_get
#define process_current           black_process_current
#define process_set_current       black_process_set_current
#define process_count             black_process_count
#define process_iterate           black_process_iterate
#define process_state_name        black_process_state_name
#define process_fork              black_process_fork
#define process_exec              black_process_exec
#define process_signal            black_process_signal
#define process_check_signals     black_process_check_signals
#define process_reparent_children black_process_reparent_children

#endif
