#include <kernel/kernel.h>
#include <net/net.h>

typedef struct {
    uint32_t black_dst;
    uint32_t black_gateway;
    uint32_t black_mask;
    int black_iface;
} black_route_entry_t;

static black_route_entry_t black_routes[32];
static int black_route_count = 0;

void black_route_init(void) {
    black_route_count = 0;
}

int black_route_add(uint32_t black_dst, uint32_t black_mask, uint32_t black_gw, int black_iface) {
    if (black_route_count >= 32) return -1;
    black_routes[black_route_count].black_dst = black_dst;
    black_routes[black_route_count].black_mask = black_mask;
    black_routes[black_route_count].black_gateway = black_gw;
    black_routes[black_route_count].black_iface = black_iface;
    black_route_count++;
    return 0;
}

uint32_t black_route_lookup(uint32_t black_dst) {
    for (int black_i = 0; black_i < black_route_count; black_i++) {
        if ((black_dst & black_routes[black_i].black_mask) == black_routes[black_i].black_dst) {
            return black_routes[black_i].black_gateway;
        }
    }
    return 0;
}
