#include <drivers/ata.h>
#include <kernel/kernel.h>
#include <string.h>

static black_ata_device_t black_ata_devices[4];
static int black_ata_count = 0;

static void black_ata_400ns_delay(uint16_t black_io) { for (int black_i = 0; black_i < 4; black_i++) black_inb(black_io + 0x206); }

static int black_ata_poll(uint16_t black_io)
{
    black_ata_400ns_delay(black_io);
    uint8_t black_status;
    int black_timeout = 100000;
    while (--black_timeout) {
        black_status = black_inb(black_io + 7);
        if (!(black_status & 0x80)) break;
    }
    if (black_timeout == 0) return -1;
    if (black_status & 0x21) return -1;
    return 0;
}

static void black_ata_identify(uint16_t black_io, uint16_t black_ctrl, uint8_t black_slave)
{
    if (black_ata_count >= 4) return;
    black_outb(black_io + 6, black_slave ? 0xB0 : 0xA0);
    for (int black_i = 0; black_i < 4; black_i++) black_inb(black_ctrl);
    black_outb(black_io + 2, 0);
    black_outb(black_io + 3, 0);
    black_outb(black_io + 4, 0);
    black_outb(black_io + 5, 0);
    black_outb(black_io + 7, 0xEC);
    uint8_t black_status = black_inb(black_io + 7);
    if (black_status == 0) return;

    int black_timeout = 100000;
    while (--black_timeout && (black_inb(black_io + 7) & 0x80));
    if (black_timeout == 0) return;

    if (black_inb(black_io + 4) || black_inb(black_io + 5)) return;

    while (--black_timeout) {
        black_status = black_inb(black_io + 7);
        if (black_status & 0x01) return;
        if (black_status & 0x08) break;
    }
    if (black_timeout == 0) return;

    uint16_t black_ident[256];
    for (int black_i = 0; black_i < 256; black_i++) black_ident[black_i] = black_inw(black_io);

    black_ata_device_t *black_dev = &black_ata_devices[black_ata_count];
    black_dev->black_io_base = black_io;
    black_dev->black_ctrl_base = black_ctrl;
    black_dev->black_slave = black_slave;
    black_dev->black_present = 1;
    black_dev->black_sectors = (black_ident[61] << 16) | black_ident[60];

    for (int black_i = 0; black_i < 40; black_i += 2) {
        black_dev->black_model[black_i] = (char)(black_ident[27 + black_i / 2] >> 8);
        black_dev->black_model[black_i + 1] = (char)(black_ident[27 + black_i / 2] & 0xFF);
    }
    black_dev->black_model[40] = '\0';
    int black_end = 39;
    while (black_end >= 0 && black_dev->black_model[black_end] == ' ') black_dev->black_model[black_end--] = '\0';
    black_ata_count++;
}

void black_ata_init(void)
{
    black_ata_count = 0;
    memset(black_ata_devices, 0, sizeof(black_ata_devices));
    black_ata_identify(ATA_PRIMARY_IO, ATA_PRIMARY_CTRL, 0);
    black_ata_identify(ATA_PRIMARY_IO, ATA_PRIMARY_CTRL, 1);
    black_ata_identify(ATA_SECONDARY_IO, ATA_SECONDARY_CTRL, 0);
    black_ata_identify(ATA_SECONDARY_IO, ATA_SECONDARY_CTRL, 1);
}

int black_ata_read_sectors(black_ata_device_t *black_dev, uint32_t black_lba, uint8_t black_count, void *black_buf)
{
    if (!black_dev || !black_dev->black_present) return -1;
    uint16_t black_io = black_dev->black_io_base;
    black_outb(black_io + 6, 0xE0 | (black_dev->black_slave << 4) | ((black_lba >> 24) & 0x0F));
    black_outb(black_io + 2, black_count);
    black_outb(black_io + 3, (uint8_t)(black_lba & 0xFF));
    black_outb(black_io + 4, (uint8_t)((black_lba >> 8) & 0xFF));
    black_outb(black_io + 5, (uint8_t)((black_lba >> 16) & 0xFF));
    black_outb(black_io + 7, 0x20);
    uint16_t *black_wbuf = (uint16_t *)black_buf;
    for (int black_s = 0; black_s < black_count; black_s++) {
        if (black_ata_poll(black_io) < 0) return -1;
        for (int black_i = 0; black_i < 256; black_i++) *black_wbuf++ = black_inw(black_io);
    }
    return 0;
}

int black_ata_write_sectors(black_ata_device_t *black_dev, uint32_t black_lba, uint8_t black_count, const void *black_buf)
{
    if (!black_dev || !black_dev->black_present) return -1;
    uint16_t black_io = black_dev->black_io_base;
    black_outb(black_io + 6, 0xE0 | (black_dev->black_slave << 4) | ((black_lba >> 24) & 0x0F));
    black_outb(black_io + 2, black_count);
    black_outb(black_io + 3, (uint8_t)(black_lba & 0xFF));
    black_outb(black_io + 4, (uint8_t)((black_lba >> 8) & 0xFF));
    black_outb(black_io + 5, (uint8_t)((black_lba >> 16) & 0xFF));
    black_outb(black_io + 7, 0x30);
    const uint16_t *black_wbuf = (const uint16_t *)black_buf;
    for (int black_s = 0; black_s < black_count; black_s++) {
        if (black_ata_poll(black_io) < 0) return -1;
        for (int black_i = 0; black_i < 256; black_i++) black_outw(black_io, *black_wbuf++);
    }
    black_outb(black_io + 7, 0xE7);
    black_ata_poll(black_io);
    return 0;
}

black_ata_device_t *black_ata_get_device(int black_idx) { return (black_idx >= 0 && black_idx < black_ata_count) ? &black_ata_devices[black_idx] : NULL; }
int black_ata_get_device_count(void) { return black_ata_count; }
