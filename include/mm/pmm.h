#ifndef _BLACK_PMM_H
#define _BLACK_PMM_H

#include <stdint.h>
#include <kernel/multiboot.h>

#define BLACK_PAGE_SIZE      4096
#define BLACK_BUDDY_MAX_ORDER 10

void     black_pmm_init(black_multiboot_info_t *black_mboot);
uint32_t black_pmm_alloc_frame(void);
void     black_pmm_free_frame(uint32_t black_addr);
uint32_t black_pmm_alloc_frames(uint32_t black_count);
void     black_pmm_free_frames(uint32_t black_addr, uint32_t black_count);
uint32_t black_pmm_get_total_memory(void);
uint32_t black_pmm_get_free_memory(void);
uint32_t black_pmm_get_used_memory(void);

#define pmm_init           black_pmm_init
#define pmm_alloc_frame    black_pmm_alloc_frame
#define pmm_free_frame     black_pmm_free_frame
#define pmm_get_total_memory black_pmm_get_total_memory
#define pmm_get_free_memory  black_pmm_get_free_memory
#define pmm_get_used_memory  black_pmm_get_used_memory

#endif
