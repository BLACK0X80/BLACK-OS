#ifndef _BLACK_VFS_H
#define _BLACK_VFS_H

#include <stdint.h>
#include <stdbool.h>

#define VFS_FILE        0x01
#define VFS_DIRECTORY   0x02
#define VFS_CHARDEV     0x04
#define VFS_BLOCKDEV    0x08
#define VFS_PIPE        0x10
#define VFS_SYMLINK     0x20
#define VFS_MOUNTPOINT  0x40
#define VFS_MAX_NAME    128
#define VFS_MAX_PATH    256

struct black_vfs_node;

typedef struct {
    char     black_name[VFS_MAX_NAME];
    uint32_t black_inode;
    uint8_t  black_type;
} black_dirent_t;

typedef black_dirent_t dirent_t;

typedef int (*black_read_fn)(struct black_vfs_node *, uint32_t, uint32_t, uint8_t *);
typedef int (*black_write_fn)(struct black_vfs_node *, uint32_t, uint32_t, uint8_t *);
typedef int (*black_open_fn)(struct black_vfs_node *, uint32_t);
typedef int (*black_close_fn)(struct black_vfs_node *);
typedef black_dirent_t *(*black_readdir_fn)(struct black_vfs_node *, uint32_t);
typedef struct black_vfs_node *(*black_finddir_fn)(struct black_vfs_node *, const char *);
typedef int (*black_create_fn)(struct black_vfs_node *, const char *, uint32_t);
typedef int (*black_unlink_fn)(struct black_vfs_node *, const char *);

typedef struct black_vfs_node {
    char     black_name[VFS_MAX_NAME];
    uint32_t black_flags;
    uint32_t black_inode;
    uint32_t black_length;
    uint32_t black_impl;
    void    *black_private;
    struct black_vfs_node *black_parent;
    struct black_vfs_node *black_ptr;
    black_read_fn    read;
    black_write_fn   write;
    black_open_fn    open;
    black_close_fn   close;
    black_readdir_fn readdir;
    black_finddir_fn finddir;
    black_create_fn  create;
    black_unlink_fn  unlink;
} black_vfs_node_t;

typedef black_vfs_node_t vfs_node_t;

void black_vfs_init(void);
int  black_vfs_read(black_vfs_node_t *black_node, uint32_t black_off, uint32_t black_size, uint8_t *black_buf);
int  black_vfs_write(black_vfs_node_t *black_node, uint32_t black_off, uint32_t black_size, uint8_t *black_buf);
int  black_vfs_open(black_vfs_node_t *black_node, uint32_t black_flags);
int  black_vfs_close(black_vfs_node_t *black_node);
black_dirent_t *black_vfs_readdir(black_vfs_node_t *black_node, uint32_t black_idx);
black_vfs_node_t *black_vfs_finddir(black_vfs_node_t *black_node, const char *black_name);
int  black_vfs_create(black_vfs_node_t *black_dir, const char *black_name, uint32_t black_type);
int  black_vfs_unlink(black_vfs_node_t *black_dir, const char *black_name);
int  black_vfs_append(black_vfs_node_t *black_node, uint32_t black_size, uint8_t *black_buf);
int  black_vfs_truncate(black_vfs_node_t *black_node);
int  black_vfs_mkdir(const char *black_path);
int  black_vfs_mount(const char *black_path, black_vfs_node_t *black_root);
int  black_vfs_unmount(const char *black_path);

black_vfs_node_t *black_vfs_get_root(void);
void black_vfs_set_root(black_vfs_node_t *black_root);
black_vfs_node_t *black_vfs_lookup(const char *black_path);
bool black_vfs_is_directory(black_vfs_node_t *black_node);

#define vfs_init      black_vfs_init
#define vfs_read      black_vfs_read
#define vfs_write     black_vfs_write
#define vfs_open      black_vfs_open
#define vfs_close     black_vfs_close
#define vfs_readdir   black_vfs_readdir
#define vfs_finddir   black_vfs_finddir
#define vfs_create    black_vfs_create
#define vfs_unlink    black_vfs_unlink
#define vfs_append    black_vfs_append
#define vfs_truncate  black_vfs_truncate
#define vfs_mkdir     black_vfs_mkdir
#define vfs_mount     black_vfs_mount
#define vfs_unmount   black_vfs_unmount
#define vfs_get_root  black_vfs_get_root
#define vfs_set_root  black_vfs_set_root
#define vfs_lookup    black_vfs_lookup
#define vfs_is_directory black_vfs_is_directory

#endif
