#include <kernel/kernel.h>
#include <kernel/process.h>

typedef struct {
    uint32_t black_mnt_ns;
    uint32_t black_pid_ns;
    uint32_t black_net_ns;
} black_namespace_t;

static black_namespace_t black_namespaces[MAX_PROCESSES];

void black_isolation_init(void) {
    for (int black_i = 0; black_i < MAX_PROCESSES; black_i++) {
        black_namespaces[black_i].black_mnt_ns = 0;
        black_namespaces[black_i].black_pid_ns = 0;
        black_namespaces[black_i].black_net_ns = 0;
    }
}

int black_isolation_clone(uint32_t black_pid, uint32_t black_flags) {
    (void)black_pid; (void)black_flags;
    return 0;
}
