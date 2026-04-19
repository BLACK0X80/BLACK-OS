#include <fs/vfs.h>
#include <kernel/kernel.h>
#include <string.h>

static black_vfs_node_t *black_vfs_root = NULL;

#define BLACK_MAX_MOUNTS 16

typedef struct {
    char black_path[VFS_MAX_PATH];
    black_vfs_node_t *black_root;
    int black_active;
} black_mount_point_t;

static black_mount_point_t black_mounts[BLACK_MAX_MOUNTS];

void black_vfs_init(void)
{
    black_vfs_root = NULL;
    memset(black_mounts, 0, sizeof(black_mounts));
}

black_vfs_node_t *black_vfs_get_root(void) { return black_vfs_root; }
void black_vfs_set_root(black_vfs_node_t *black_r) { black_vfs_root = black_r; }

int black_vfs_read(black_vfs_node_t *black_n, uint32_t black_off, uint32_t black_sz, uint8_t *black_buf)
{
    if (black_n && black_n->read) return black_n->read(black_n, black_off, black_sz, black_buf);
    return -1;
}

int black_vfs_write(black_vfs_node_t *black_n, uint32_t black_off, uint32_t black_sz, uint8_t *black_buf)
{
    if (black_n && black_n->write) return black_n->write(black_n, black_off, black_sz, black_buf);
    return -1;
}

int black_vfs_open(black_vfs_node_t *black_n, uint32_t black_flags)
{
    if (black_n && black_n->open) return black_n->open(black_n, black_flags);
    return 0;
}

int black_vfs_close(black_vfs_node_t *black_n)
{
    if (black_n && black_n->close) return black_n->close(black_n);
    return 0;
}

black_dirent_t *black_vfs_readdir(black_vfs_node_t *black_n, uint32_t black_idx)
{
    if (black_n && black_n->readdir) return black_n->readdir(black_n, black_idx);
    return NULL;
}

black_vfs_node_t *black_vfs_finddir(black_vfs_node_t *black_n, const char *black_name)
{
    if (black_n && black_n->finddir) return black_n->finddir(black_n, black_name);
    return NULL;
}

int black_vfs_create(black_vfs_node_t *black_dir, const char *black_name, uint32_t black_type)
{
    if (black_dir && black_dir->create) return black_dir->create(black_dir, black_name, black_type);
    return -1;
}

int black_vfs_unlink(black_vfs_node_t *black_dir, const char *black_name)
{
    if (black_dir && black_dir->unlink) return black_dir->unlink(black_dir, black_name);
    return -1;
}

int black_vfs_append(black_vfs_node_t *black_n, uint32_t black_sz, uint8_t *black_buf)
{
    if (!black_n || !black_n->write) return -1;
    return black_n->write(black_n, black_n->black_length, black_sz, black_buf);
}

int black_vfs_truncate(black_vfs_node_t *black_n)
{
    if (!black_n) return -1;
    black_n->black_length = 0;
    return 0;
}

bool black_vfs_is_directory(black_vfs_node_t *black_n)
{
    return black_n && (black_n->black_flags & VFS_DIRECTORY);
}

static black_vfs_node_t *black_vfs_traverse(const char *black_path)
{
    if (!black_path || black_path[0] != '/') return NULL;
    if (strcmp(black_path, "/") == 0) return black_vfs_root;

    black_vfs_node_t *black_cur = black_vfs_root;
    char black_buf[VFS_MAX_PATH];
    strncpy(black_buf, black_path, VFS_MAX_PATH - 1);
    black_buf[VFS_MAX_PATH - 1] = '\0';

    char *black_tok = black_buf + 1;
    while (*black_tok && black_cur) {
        char *black_next = strchr(black_tok, '/');
        if (black_next) *black_next = '\0';
        if (strlen(black_tok) > 0)
            black_cur = black_vfs_finddir(black_cur, black_tok);
        if (black_next) black_tok = black_next + 1;
        else break;
    }
    return black_cur;
}

black_vfs_node_t *black_vfs_lookup(const char *black_path)
{
    for (int black_i = 0; black_i < BLACK_MAX_MOUNTS; black_i++) {
        if (!black_mounts[black_i].black_active) continue;
        size_t black_mlen = strlen(black_mounts[black_i].black_path);
        if (strncmp(black_path, black_mounts[black_i].black_path, black_mlen) == 0) {
            const char *black_rest = black_path + black_mlen;
            if (*black_rest == '\0' || *black_rest == '/') {
                if (*black_rest == '\0') return black_mounts[black_i].black_root;
                char black_sub[VFS_MAX_PATH] = "/";
                strcat(black_sub, black_rest + 1);
                black_vfs_node_t *black_old = black_vfs_root;
                black_vfs_root = black_mounts[black_i].black_root;
                black_vfs_node_t *black_r = black_vfs_traverse(black_sub);
                black_vfs_root = black_old;
                return black_r;
            }
        }
    }
    return black_vfs_traverse(black_path);
}

int black_vfs_mount(const char *black_path, black_vfs_node_t *black_r)
{
    for (int black_i = 0; black_i < BLACK_MAX_MOUNTS; black_i++) {
        if (!black_mounts[black_i].black_active) {
            strncpy(black_mounts[black_i].black_path, black_path, VFS_MAX_PATH - 1);
            black_mounts[black_i].black_root = black_r;
            black_mounts[black_i].black_active = 1;
            return 0;
        }
    }
    return -1;
}

int black_vfs_unmount(const char *black_path)
{
    for (int black_i = 0; black_i < BLACK_MAX_MOUNTS; black_i++) {
        if (black_mounts[black_i].black_active && strcmp(black_mounts[black_i].black_path, black_path) == 0) {
            black_mounts[black_i].black_active = 0;
            return 0;
        }
    }
    return -1;
}

int black_vfs_mkdir(const char *black_path)
{
    char black_dir[VFS_MAX_PATH], black_name[VFS_MAX_NAME];
    strncpy(black_dir, black_path, VFS_MAX_PATH - 1);
    char *black_last = strrchr(black_dir, '/');
    if (!black_last) return -1;
    strncpy(black_name, black_last + 1, VFS_MAX_NAME - 1);
    if (black_last == black_dir) black_dir[1] = '\0';
    else *black_last = '\0';
    black_vfs_node_t *black_parent = black_vfs_lookup(black_dir);
    if (!black_parent) return -1;
    return black_vfs_create(black_parent, black_name, VFS_DIRECTORY);
}
