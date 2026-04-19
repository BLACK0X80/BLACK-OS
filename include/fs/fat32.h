#ifndef _BLACK_FAT32_H
#define _BLACK_FAT32_H

#include <fs/vfs.h>

void black_fat32_automount_all(void);

#define fat32_automount_all black_fat32_automount_all

#endif
