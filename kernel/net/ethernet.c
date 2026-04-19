#include <kernel/kernel.h>
#include <net/net.h>

void black_ethernet_init(void) {
}

int black_ethernet_send(const uint8_t *black_dst_mac, uint16_t black_type, const void *black_data, uint16_t black_len) {
    (void)black_dst_mac; (void)black_type; (void)black_data; (void)black_len;
    return 0;
}

void black_ethernet_receive(void *black_frame, uint16_t black_len) {
    (void)black_frame; (void)black_len;
}
