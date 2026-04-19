#include <fs/ramfs.h>
#include <kernel/kernel.h>
#include <string.h>

#define BLACK_RAMFS_MAX_CHILDREN 64
#define BLACK_RAMFS_MAX_FILESIZE (1024 * 1024)

typedef struct black_ramfs_entry {
    black_vfs_node_t black_node;
    uint8_t *black_data;
    uint32_t black_capacity;
    struct black_ramfs_entry *black_children[BLACK_RAMFS_MAX_CHILDREN];
    uint32_t black_child_count;
    struct black_ramfs_entry *black_parent_entry;
} black_ramfs_entry_t;

static int black_ramfs_read(black_vfs_node_t *black_n, uint32_t black_off, uint32_t black_sz, uint8_t *black_buf)
{
    black_ramfs_entry_t *black_e = (black_ramfs_entry_t *)black_n;
    if (!black_e->black_data || black_off >= black_n->black_length) return 0;
    uint32_t black_avail = black_n->black_length - black_off;
    if (black_sz > black_avail) black_sz = black_avail;
    memcpy(black_buf, black_e->black_data + black_off, black_sz);
    return (int)black_sz;
}

static int black_ramfs_write(black_vfs_node_t *black_n, uint32_t black_off, uint32_t black_sz, uint8_t *black_buf)
{
    black_ramfs_entry_t *black_e = (black_ramfs_entry_t *)black_n;
    uint32_t black_needed = black_off + black_sz;
    if (black_needed > BLACK_RAMFS_MAX_FILESIZE) return -1;
    if (black_needed > black_e->black_capacity) {
        uint32_t black_newcap = black_e->black_capacity ? black_e->black_capacity : 512;
        while (black_newcap < black_needed) black_newcap *= 2;
        if (black_newcap > BLACK_RAMFS_MAX_FILESIZE) black_newcap = BLACK_RAMFS_MAX_FILESIZE;
        uint8_t *black_newdata = kmalloc(black_newcap);
        if (!black_newdata) return -1;
        if (black_e->black_data) { memcpy(black_newdata, black_e->black_data, black_n->black_length); kfree(black_e->black_data); }
        black_e->black_data = black_newdata;
        black_e->black_capacity = black_newcap;
    }
    memcpy(black_e->black_data + black_off, black_buf, black_sz);
    if (black_off + black_sz > black_n->black_length) black_n->black_length = black_off + black_sz;
    return (int)black_sz;
}

static black_dirent_t *black_ramfs_readdir(black_vfs_node_t *black_n, uint32_t black_idx)
{
    static black_dirent_t black_de;
    black_ramfs_entry_t *black_e = (black_ramfs_entry_t *)black_n;
    if (black_idx >= black_e->black_child_count) return NULL;
    strncpy(black_de.black_name, black_e->black_children[black_idx]->black_node.black_name, VFS_MAX_NAME);
    black_de.black_inode = black_e->black_children[black_idx]->black_node.black_inode;
    black_de.black_type = black_e->black_children[black_idx]->black_node.black_flags;
    return &black_de;
}

static black_vfs_node_t *black_ramfs_finddir(black_vfs_node_t *black_n, const char *black_name)
{
    black_ramfs_entry_t *black_e = (black_ramfs_entry_t *)black_n;
    for (uint32_t black_i = 0; black_i < black_e->black_child_count; black_i++) {
        if (strcmp(black_e->black_children[black_i]->black_node.black_name, black_name) == 0)
            return &black_e->black_children[black_i]->black_node;
    }
    return NULL;
}

static uint32_t black_ramfs_next_inode = 1;

static int black_ramfs_create(black_vfs_node_t *black_n, const char *black_name, uint32_t black_type)
{
    black_ramfs_entry_t *black_e = (black_ramfs_entry_t *)black_n;
    if (black_e->black_child_count >= BLACK_RAMFS_MAX_CHILDREN) return -1;
    black_ramfs_entry_t *black_child = kcalloc(1, sizeof(black_ramfs_entry_t));
    if (!black_child) return -1;
    strncpy(black_child->black_node.black_name, black_name, VFS_MAX_NAME - 1);
    black_child->black_node.black_flags = black_type;
    black_child->black_node.black_inode = black_ramfs_next_inode++;
    black_child->black_node.black_parent = black_n;
    black_child->black_parent_entry = black_e;
    if (black_type & VFS_DIRECTORY) {
        black_child->black_node.readdir = black_ramfs_readdir;
        black_child->black_node.finddir = black_ramfs_finddir;
        black_child->black_node.create = black_ramfs_create;
        black_child->black_node.unlink = NULL;
    } else {
        black_child->black_node.read = black_ramfs_read;
        black_child->black_node.write = black_ramfs_write;
    }
    black_e->black_children[black_e->black_child_count++] = black_child;
    return 0;
}

black_vfs_node_t *black_ramfs_init(void)
{
    black_ramfs_entry_t *black_root = kcalloc(1, sizeof(black_ramfs_entry_t));
    if (!black_root) return NULL;
    strcpy(black_root->black_node.black_name, "/");
    black_root->black_node.black_flags = VFS_DIRECTORY;
    black_root->black_node.black_inode = black_ramfs_next_inode++;
    black_root->black_node.readdir = black_ramfs_readdir;
    black_root->black_node.finddir = black_ramfs_finddir;
    black_root->black_node.create = black_ramfs_create;
    return &black_root->black_node;
}

black_vfs_node_t *black_ramfs_create_dir(black_vfs_node_t *black_p, const char *black_name)
{
    if (black_ramfs_create(black_p, black_name, VFS_DIRECTORY) < 0) return NULL;
    return black_vfs_finddir(black_p, black_name);
}

black_vfs_node_t *black_ramfs_create_file(black_vfs_node_t *black_p, const char *black_name)
{
    if (black_ramfs_create(black_p, black_name, VFS_FILE) < 0) return NULL;
    return black_vfs_finddir(black_p, black_name);
}
