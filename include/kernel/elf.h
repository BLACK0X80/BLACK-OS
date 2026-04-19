#ifndef _BLACK_ELF_H
#define _BLACK_ELF_H
#include <stdint.h>
int black_elf_load(const void *black_data, uint32_t black_size);
uint32_t black_elf_get_entry(const void *black_data);
#endif
