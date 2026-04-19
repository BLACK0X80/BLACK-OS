#ifndef _BLACK_VMM_H
#define _BLACK_VMM_H

#include <stdint.h>

#define PAGE_SIZE     4096
#define PAGE_PRESENT  0x001
#define PAGE_WRITE    0x002
#define PAGE_USER     0x004
#define PAGE_4MB      0x080
#define PAGE_NOCACHE  0x010
#define PAGE_COW      0x200

void     black_vmm_init(void);
void     black_vmm_map_page(uint32_t black_virt, uint32_t black_phys, uint32_t black_flags);
void     black_vmm_unmap_page(uint32_t black_virt);
uint32_t black_vmm_get_physical(uint32_t black_virt);
int      black_vmm_is_mapped(uint32_t black_virt);
void     black_vmm_flush_tlb(void);
void     black_vmm_invlpg(uint32_t black_virt);
uint32_t *black_vmm_get_current_pagedir(void);
void     black_vmm_map_range(uint32_t black_virt, uint32_t black_phys, uint32_t black_size, uint32_t black_flags);
int      black_vmm_handle_page_fault(uint32_t black_addr, uint32_t black_err);

#define vmm_init          black_vmm_init
#define vmm_map_page      black_vmm_map_page
#define vmm_unmap_page    black_vmm_unmap_page
#define vmm_get_physical  black_vmm_get_physical
#define vmm_is_mapped     black_vmm_is_mapped
#define vmm_flush_tlb     black_vmm_flush_tlb
#define vmm_invlpg        black_vmm_invlpg

#endif
