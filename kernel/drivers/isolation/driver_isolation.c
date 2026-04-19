#include <kernel/kernel.h>
#include <kernel/process.h>

void black_isolation_init(void) {
}

int black_driver_isolate(uint32_t black_pid) {
    (void)black_pid;
    return 0;
}
