#ifndef _BLACK_E1000_H
#define _BLACK_E1000_H

#include <stdint.h>
#include <drivers/pci.h>

void black_e1000_init(black_pci_device_t *black_dev);
int black_e1000_send(const void *black_data, uint16_t black_len);
int black_e1000_receive(void *black_buf, uint16_t black_max_len);

#endif
