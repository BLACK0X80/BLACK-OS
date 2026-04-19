#ifndef _BLACK_ATA_H
#define _BLACK_ATA_H

#include <stdint.h>

#define ATA_PRIMARY_IO    0x1F0
#define ATA_SECONDARY_IO  0x170
#define ATA_PRIMARY_CTRL  0x3F6
#define ATA_SECONDARY_CTRL 0x376

typedef struct {
    uint16_t black_io_base;
    uint16_t black_ctrl_base;
    uint8_t  black_slave;
    uint8_t  black_present;
    uint32_t black_sectors;
    char     black_model[41];
} black_ata_device_t;

typedef black_ata_device_t ata_device_t;

void black_ata_init(void);
int  black_ata_read_sectors(black_ata_device_t *black_dev, uint32_t black_lba, uint8_t black_count, void *black_buf);
int  black_ata_write_sectors(black_ata_device_t *black_dev, uint32_t black_lba, uint8_t black_count, const void *black_buf);
black_ata_device_t *black_ata_get_device(int black_idx);
int  black_ata_get_device_count(void);

#define ata_init           black_ata_init
#define ata_read_sectors   black_ata_read_sectors
#define ata_write_sectors  black_ata_write_sectors
#define ata_get_device     black_ata_get_device
#define ata_get_device_count black_ata_get_device_count

#endif
