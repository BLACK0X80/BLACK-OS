#include <kernel/kernel.h>
#include <fs/vfs.h>
#include <fs/fat32.h>

int black_fat32_vfs_read(black_vfs_node_t *black_node, uint32_t black_off, uint32_t black_size, uint8_t *black_buf) {
    (void)black_node; (void)black_off; (void)black_size; (void)black_buf;
    return 0;
}

int black_fat32_vfs_write(black_vfs_node_t *black_node, uint32_t black_off, uint32_t black_size, const uint8_t *black_buf) {
    (void)black_node; (void)black_off; (void)black_size; (void)black_buf;
    return 0;
}

black_vfs_node_t *black_fat32_vfs_mount(void) {
    return 0;
}
