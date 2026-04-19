#include <kernel/kernel.h>
#include <drivers/ata.h>
#include <drivers/pci.h>

void black_ata_pci_init(void) {
    for (uint32_t black_i = 0; black_i < black_pci_get_device_count(); black_i++) {
        black_pci_device_t *black_dev = black_pci_get_device(black_i);
        if (black_dev->black_class_code == 0x01 && black_dev->black_subclass == 0x01) {
            black_ata_init();
            return;
        }
    }
}
