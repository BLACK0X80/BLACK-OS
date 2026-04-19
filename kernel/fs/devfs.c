#include <fs/vfs.h>
#include <kernel/kernel.h>
#include <drivers/pit.h>
#include <mm/pmm.h>
#include <mm/heap.h>
#include <kernel/scheduler.h>
#include <string.h>

static black_vfs_node_t *black_devfs_root = NULL;

static int black_devfs_null_read(black_vfs_node_t *black_n, uint32_t black_off, uint32_t black_sz, uint8_t *black_buf)
{
    (void)black_n; (void)black_off;
    memset(black_buf, 0, black_sz);
    return 0;
}

static int black_devfs_null_write(black_vfs_node_t *black_n, uint32_t black_off, uint32_t black_sz, uint8_t *black_buf)
{
    (void)black_n; (void)black_off; (void)black_buf;
    return (int)black_sz;
}

static int black_devfs_zero_read(black_vfs_node_t *black_n, uint32_t black_off, uint32_t black_sz, uint8_t *black_buf)
{
    (void)black_n; (void)black_off;
    memset(black_buf, 0, black_sz);
    return (int)black_sz;
}

static int black_devfs_random_read(black_vfs_node_t *black_n, uint32_t black_off, uint32_t black_sz, uint8_t *black_buf)
{
    (void)black_n; (void)black_off;
    uint32_t black_seed = (uint32_t)black_rdtsc();
    for (uint32_t black_i = 0; black_i < black_sz; black_i++) {
        black_seed = black_seed * 1103515245 + 12345;
        black_buf[black_i] = (uint8_t)(black_seed >> 16);
    }
    return (int)black_sz;
}

typedef struct {
    black_vfs_node_t black_node;
    int (*black_dev_read)(black_vfs_node_t *, uint32_t, uint32_t, uint8_t *);
    int (*black_dev_write)(black_vfs_node_t *, uint32_t, uint32_t, uint8_t *);
} black_devfs_entry_t;

#define BLACK_MAX_DEVFS_ENTRIES 16
static black_devfs_entry_t black_devfs_entries[BLACK_MAX_DEVFS_ENTRIES];
static int black_devfs_count = 0;

static black_dirent_t *black_devfs_readdir(black_vfs_node_t *black_n, uint32_t black_idx)
{
    (void)black_n;
    static black_dirent_t black_de;
    if (black_idx >= (uint32_t)black_devfs_count) return NULL;
    strncpy(black_de.black_name, black_devfs_entries[black_idx].black_node.black_name, VFS_MAX_NAME);
    black_de.black_inode = black_devfs_entries[black_idx].black_node.black_inode;
    black_de.black_type = VFS_CHARDEV;
    return &black_de;
}

static black_vfs_node_t *black_devfs_finddir(black_vfs_node_t *black_n, const char *black_name)
{
    (void)black_n;
    for (int black_i = 0; black_i < black_devfs_count; black_i++) {
        if (strcmp(black_devfs_entries[black_i].black_node.black_name, black_name) == 0)
            return &black_devfs_entries[black_i].black_node;
    }
    return NULL;
}

static void black_devfs_add(const char *black_name, int (*black_r)(black_vfs_node_t*,uint32_t,uint32_t,uint8_t*), int (*black_w)(black_vfs_node_t*,uint32_t,uint32_t,uint8_t*))
{
    if (black_devfs_count >= BLACK_MAX_DEVFS_ENTRIES) return;
    black_devfs_entry_t *black_e = &black_devfs_entries[black_devfs_count++];
    memset(black_e, 0, sizeof(black_devfs_entry_t));
    strncpy(black_e->black_node.black_name, black_name, VFS_MAX_NAME - 1);
    black_e->black_node.black_flags = VFS_CHARDEV;
    black_e->black_node.black_inode = 1000 + black_devfs_count;
    black_e->black_node.read = black_r;
    black_e->black_node.write = black_w;
}

void black_devfs_init(black_vfs_node_t *black_root)
{
    black_devfs_root = kmalloc(sizeof(black_vfs_node_t));
    memset(black_devfs_root, 0, sizeof(black_vfs_node_t));
    strcpy(black_devfs_root->black_name, "dev");
    black_devfs_root->black_flags = VFS_DIRECTORY;
    black_devfs_root->black_inode = 999;
    black_devfs_root->readdir = black_devfs_readdir;
    black_devfs_root->finddir = black_devfs_finddir;

    black_devfs_add("null", black_devfs_null_read, black_devfs_null_write);
    black_devfs_add("zero", black_devfs_zero_read, black_devfs_null_write);
    black_devfs_add("random", black_devfs_random_read, NULL);
    black_devfs_add("urandom", black_devfs_random_read, NULL);

    black_vfs_mount("/dev", black_devfs_root);
    (void)black_root;
}
