#include <kernel/kernel.h>
#include <fs/vfs.h>

#define BLACK_FAT32_SECTOR_SIZE 512

typedef struct {
    uint8_t  black_jmp[3];
    char     black_oem[8];
    uint16_t black_bytes_per_sector;
    uint8_t  black_sectors_per_cluster;
    uint16_t black_reserved_sectors;
    uint8_t  black_num_fats;
    uint16_t black_root_entry_count;
    uint16_t black_total_sectors_16;
    uint8_t  black_media_type;
    uint16_t black_fat_size_16;
    uint16_t black_sectors_per_track;
    uint16_t black_num_heads;
    uint32_t black_hidden_sectors;
    uint32_t black_total_sectors_32;
    uint32_t black_fat_size_32;
    uint16_t black_ext_flags;
    uint16_t black_fs_version;
    uint32_t black_root_cluster;
} __attribute__((packed)) black_fat32_bpb_t;

static black_fat32_bpb_t black_bpb;

void black_fat32_init(void) {
    (void)black_bpb;
}

int black_fat32_read_cluster(uint32_t black_cluster, void *black_buf) {
    (void)black_cluster;
    (void)black_buf;
    return 0;
}
