#include <fs/vfs.h>
#include <kernel/kernel.h>
#include <kernel/scheduler.h>
#include <kernel/process.h>
#include <mm/pmm.h>
#include <mm/heap.h>
#include <drivers/pit.h>
#include <string.h>

static black_vfs_node_t *black_procfs_root = NULL;

static int black_procfs_uptime_read(black_vfs_node_t *black_n, uint32_t black_off, uint32_t black_sz, uint8_t *black_buf)
{
    (void)black_n;
    char black_tmp[64];
    uint64_t black_ms = black_pit_get_uptime_ms();
    uint32_t black_secs = (uint32_t)(black_ms / 1000);
    int black_len = 0;
    uint32_t black_val = black_secs;
    char black_num[12]; int black_ni = 0;
    if (black_val == 0) black_num[black_ni++] = '0';
    else while (black_val > 0) { black_num[black_ni++] = '0' + (black_val % 10); black_val /= 10; }
    while (black_ni > 0) black_tmp[black_len++] = black_num[--black_ni];
    black_tmp[black_len++] = '\n';
    black_tmp[black_len] = '\0';
    if (black_off >= (uint32_t)black_len) return 0;
    uint32_t black_avail = (uint32_t)black_len - black_off;
    if (black_sz > black_avail) black_sz = black_avail;
    memcpy(black_buf, black_tmp + black_off, black_sz);
    return (int)black_sz;
}

static int black_procfs_meminfo_read(black_vfs_node_t *black_n, uint32_t black_off, uint32_t black_sz, uint8_t *black_buf)
{
    (void)black_n;
    char black_tmp[256];
    uint32_t black_total = black_pmm_get_total_memory() / 1024;
    uint32_t black_free = black_pmm_get_free_memory() / 1024;
    uint32_t black_used = black_pmm_get_used_memory() / 1024;
    int black_len = 0;
    memcpy(black_tmp, "MemTotal: ", 10); black_len = 10;
    char black_num[12]; int black_ni = 0;
    uint32_t black_v = black_total;
    if (black_v == 0) black_num[black_ni++] = '0';
    else while (black_v > 0) { black_num[black_ni++] = '0' + (black_v % 10); black_v /= 10; }
    while (black_ni > 0) black_tmp[black_len++] = black_num[--black_ni];
    memcpy(black_tmp + black_len, " kB\nMemFree: ", 13); black_len += 13;
    black_ni = 0; black_v = black_free;
    if (black_v == 0) black_num[black_ni++] = '0';
    else while (black_v > 0) { black_num[black_ni++] = '0' + (black_v % 10); black_v /= 10; }
    while (black_ni > 0) black_tmp[black_len++] = black_num[--black_ni];
    memcpy(black_tmp + black_len, " kB\nMemUsed: ", 13); black_len += 13;
    black_ni = 0; black_v = black_used;
    if (black_v == 0) black_num[black_ni++] = '0';
    else while (black_v > 0) { black_num[black_ni++] = '0' + (black_v % 10); black_v /= 10; }
    while (black_ni > 0) black_tmp[black_len++] = black_num[--black_ni];
    memcpy(black_tmp + black_len, " kB\n", 4); black_len += 4;
    black_tmp[black_len] = '\0';
    if (black_off >= (uint32_t)black_len) return 0;
    uint32_t black_avail = (uint32_t)black_len - black_off;
    if (black_sz > black_avail) black_sz = black_avail;
    memcpy(black_buf, black_tmp + black_off, black_sz);
    return (int)black_sz;
}

static int black_procfs_version_read(black_vfs_node_t *black_n, uint32_t black_off, uint32_t black_sz, uint8_t *black_buf)
{
    (void)black_n;
    const char *black_ver = "BlackOS PHANTOM v" BLACK_VERSION_STRING "\n";
    uint32_t black_len = strlen(black_ver);
    if (black_off >= black_len) return 0;
    uint32_t black_avail = black_len - black_off;
    if (black_sz > black_avail) black_sz = black_avail;
    memcpy(black_buf, black_ver + black_off, black_sz);
    return (int)black_sz;
}

static black_vfs_node_t black_procfs_entries[8];
static int black_procfs_ecount = 0;

static void black_procfs_add(const char *black_name, int (*black_r)(black_vfs_node_t*,uint32_t,uint32_t,uint8_t*))
{
    if (black_procfs_ecount >= 8) return;
    black_vfs_node_t *black_e = &black_procfs_entries[black_procfs_ecount++];
    memset(black_e, 0, sizeof(black_vfs_node_t));
    strncpy(black_e->black_name, black_name, VFS_MAX_NAME - 1);
    black_e->black_flags = VFS_FILE;
    black_e->black_inode = 2000 + black_procfs_ecount;
    black_e->read = black_r;
}

static black_dirent_t *black_procfs_readdir_fn(black_vfs_node_t *black_n, uint32_t black_idx)
{
    (void)black_n;
    static black_dirent_t black_de;
    if (black_idx >= (uint32_t)black_procfs_ecount) return NULL;
    strncpy(black_de.black_name, black_procfs_entries[black_idx].black_name, VFS_MAX_NAME);
    black_de.black_inode = black_procfs_entries[black_idx].black_inode;
    black_de.black_type = VFS_FILE;
    return &black_de;
}

static black_vfs_node_t *black_procfs_finddir_fn(black_vfs_node_t *black_n, const char *black_name)
{
    (void)black_n;
    for (int black_i = 0; black_i < black_procfs_ecount; black_i++) {
        if (strcmp(black_procfs_entries[black_i].black_name, black_name) == 0)
            return &black_procfs_entries[black_i];
    }
    return NULL;
}

void black_procfs_init(black_vfs_node_t *black_root)
{
    (void)black_root;
    black_procfs_root = kmalloc(sizeof(black_vfs_node_t));
    memset(black_procfs_root, 0, sizeof(black_vfs_node_t));
    strcpy(black_procfs_root->black_name, "proc");
    black_procfs_root->black_flags = VFS_DIRECTORY;
    black_procfs_root->black_inode = 1999;
    black_procfs_root->readdir = black_procfs_readdir_fn;
    black_procfs_root->finddir = black_procfs_finddir_fn;

    black_procfs_add("uptime", black_procfs_uptime_read);
    black_procfs_add("meminfo", black_procfs_meminfo_read);
    black_procfs_add("version", black_procfs_version_read);

    black_vfs_mount("/proc", black_procfs_root);
}
