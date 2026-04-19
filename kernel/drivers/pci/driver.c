#include <kernel/kernel.h>
#include <drivers/pci.h>

void black_pci_driver_register(uint16_t black_vendor, uint16_t black_device, void (*black_init)(black_pci_device_t *)) {
    for (uint32_t black_i = 0; black_i < black_pci_get_device_count(); black_i++) {
        black_pci_device_t *black_dev = black_pci_get_device(black_i);
        if (black_dev->black_vendor_id == black_vendor && black_dev->black_device_id == black_device) {
            black_init(black_dev);
        }
    }
}
