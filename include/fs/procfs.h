#ifndef _BLACK_PROCFS_H
#define _BLACK_PROCFS_H

#include <fs/vfs.h>

void black_procfs_init(black_vfs_node_t *black_root);
#define procfs_init black_procfs_init

#endif
