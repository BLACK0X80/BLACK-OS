#ifndef _BLACK_DRIVER_H
#define _BLACK_DRIVER_H
#include <drivers/pci.h>
void black_pci_driver_register(uint16_t black_vendor, uint16_t black_device, void (*black_init)(black_pci_device_t *));
#endif
