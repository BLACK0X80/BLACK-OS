#ifndef _BLACK_DEVFS_H
#define _BLACK_DEVFS_H

#include <fs/vfs.h>

void black_devfs_init(black_vfs_node_t *black_root);
#define devfs_init black_devfs_init

#endif
