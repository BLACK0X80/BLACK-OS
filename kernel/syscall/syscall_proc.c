#include <kernel/kernel.h>

int32_t black_syscall_proc_fork(void) {
    return -1;
}

int32_t black_syscall_proc_exec(const char *black_path) {
    (void)black_path;
    return -1;
}

int32_t black_syscall_proc_exit(int black_code) {
    (void)black_code;
    return 0;
}

int32_t black_syscall_proc_waitpid(int black_pid) {
    (void)black_pid;
    return 0;
}

int32_t black_syscall_proc_getpid(void) {
    return 0;
}
