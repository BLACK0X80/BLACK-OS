#include <kernel/kernel.h>
#include <net/net.h>

#define BLACK_MAX_SOCKETS 64

typedef struct {
    int black_type;
    int black_proto;
    uint32_t black_local_ip;
    uint16_t black_local_port;
    uint32_t black_remote_ip;
    uint16_t black_remote_port;
    int black_active;
} black_socket_t;

static black_socket_t black_sockets[BLACK_MAX_SOCKETS];

void black_socket_init(void) {
    for (int black_i = 0; black_i < BLACK_MAX_SOCKETS; black_i++) {
        black_sockets[black_i].black_active = 0;
    }
}

int black_socket_create(int black_type, int black_proto) {
    for (int black_i = 0; black_i < BLACK_MAX_SOCKETS; black_i++) {
        if (!black_sockets[black_i].black_active) {
            black_sockets[black_i].black_type = black_type;
            black_sockets[black_i].black_proto = black_proto;
            black_sockets[black_i].black_active = 1;
            return black_i;
        }
    }
    return -1;
}

int black_socket_bind(int black_fd, uint32_t black_ip, uint16_t black_port) {
    if (black_fd < 0 || black_fd >= BLACK_MAX_SOCKETS) return -1;
    black_sockets[black_fd].black_local_ip = black_ip;
    black_sockets[black_fd].black_local_port = black_port;
    return 0;
}

int black_socket_close(int black_fd) {
    if (black_fd < 0 || black_fd >= BLACK_MAX_SOCKETS) return -1;
    black_sockets[black_fd].black_active = 0;
    return 0;
}
