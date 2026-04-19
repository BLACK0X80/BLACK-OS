#ifndef _BLACK_RAMFS_H
#define _BLACK_RAMFS_H

#include <fs/vfs.h>

black_vfs_node_t *black_ramfs_init(void);
black_vfs_node_t *black_ramfs_create_dir(black_vfs_node_t *black_parent, const char *black_name);
black_vfs_node_t *black_ramfs_create_file(black_vfs_node_t *black_parent, const char *black_name);

#define ramfs_init        black_ramfs_init
#define ramfs_create_dir  black_ramfs_create_dir
#define ramfs_create_file black_ramfs_create_file

#endif
