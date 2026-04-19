#include <kernel/kernel.h>

void black_capabilities_init(void) {
}

int black_capabilities_check(uint32_t black_pid, uint32_t black_cap) {
    (void)black_pid; (void)black_cap;
    return 1;
}

int black_capabilities_grant(uint32_t black_pid, uint32_t black_cap) {
    (void)black_pid; (void)black_cap;
    return 0;
}

int black_capabilities_revoke(uint32_t black_pid, uint32_t black_cap) {
    (void)black_pid; (void)black_cap;
    return 0;
}
