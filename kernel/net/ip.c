#include <kernel/kernel.h>
#include <net/net.h>

void black_ip_init(void) {
}

int black_ip_send(uint32_t black_dst, uint8_t black_proto, const void *black_data, uint16_t black_len) {
    (void)black_dst; (void)black_proto; (void)black_data; (void)black_len;
    return 0;
}

void black_ip_receive(void *black_data, uint16_t black_len) {
    (void)black_data; (void)black_len;
}
