#include <kernel/kernel.h>
#include <kernel/process.h>

void black_signal_init(void) {
}

int black_signal_send(uint32_t black_pid, int black_signum) {
    (void)black_pid;
    (void)black_signum;
    return 0;
}

void black_signal_handle(int black_signum) {
    (void)black_signum;
}
