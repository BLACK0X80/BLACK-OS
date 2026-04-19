#include <kernel/kernel.h>
#include <net/net.h>

void black_arp_init(void) {
}

int black_arp_resolve(uint32_t black_ip, uint8_t *black_mac) {
    (void)black_ip; (void)black_mac;
    return -1;
}

void black_arp_receive(void *black_data, uint16_t black_len) {
    (void)black_data; (void)black_len;
}
