#include <mm/vmm.h>
#include <mm/pmm.h>
#include <kernel/kernel.h>
#include <string.h>

#define BLACK_RECURSIVE_PD_INDEX  1023
#define BLACK_RECURSIVE_PD_ADDR  0xFFFFF000
#define BLACK_RECURSIVE_PT_BASE  0xFFC00000

#define BLACK_GET_PT(black_idx) ((uint32_t *)(BLACK_RECURSIVE_PT_BASE + ((black_idx) * PAGE_SIZE)))
#define BLACK_GET_PD()          ((uint32_t *)BLACK_RECURSIVE_PD_ADDR)

extern uint32_t black_boot_page_directory;

static uint32_t black_cow_refcount[256 * 1024];

void black_vmm_init(void)
{
    uint32_t *black_pd = (uint32_t *)((uint32_t)&black_boot_page_directory + KERNEL_VMA);
    black_pd[BLACK_RECURSIVE_PD_INDEX] =
        (uint32_t)&black_boot_page_directory | PAGE_PRESENT | PAGE_WRITE;
    black_vmm_flush_tlb();
    memset(black_cow_refcount, 0, sizeof(black_cow_refcount));
}

void black_vmm_map_page(uint32_t black_virt, uint32_t black_phys, uint32_t black_flags)
{
    uint32_t black_pd_idx = (black_virt >> 22) & 0x3FF;
    uint32_t black_pt_idx = (black_virt >> 12) & 0x3FF;
    uint32_t *black_pd = BLACK_GET_PD();

    if (!(black_pd[black_pd_idx] & PAGE_PRESENT)) {
        uint32_t black_pt_phys = black_pmm_alloc_frame();
        if (!black_pt_phys) return;
        black_pd[black_pd_idx] = black_pt_phys | PAGE_PRESENT | PAGE_WRITE | (black_flags & PAGE_USER);
        black_vmm_invlpg((uint32_t)BLACK_GET_PT(black_pd_idx));
        uint32_t *black_pt = BLACK_GET_PT(black_pd_idx);
        memset(black_pt, 0, PAGE_SIZE);
    } else if (black_flags & PAGE_USER) {
        black_pd[black_pd_idx] |= PAGE_USER;
    }

    uint32_t *black_pt = BLACK_GET_PT(black_pd_idx);
    black_pt[black_pt_idx] = (black_phys & 0xFFFFF000) | (black_flags & 0xFFF) | PAGE_PRESENT;
    black_vmm_invlpg(black_virt);
}

void black_vmm_unmap_page(uint32_t black_virt)
{
    uint32_t black_pd_idx = (black_virt >> 22) & 0x3FF;
    uint32_t black_pt_idx = (black_virt >> 12) & 0x3FF;
    uint32_t *black_pd = BLACK_GET_PD();
    if (!(black_pd[black_pd_idx] & PAGE_PRESENT)) return;
    uint32_t *black_pt = BLACK_GET_PT(black_pd_idx);
    black_pt[black_pt_idx] = 0;
    black_vmm_invlpg(black_virt);
}

uint32_t black_vmm_get_physical(uint32_t black_virt)
{
    uint32_t black_pd_idx = (black_virt >> 22) & 0x3FF;
    uint32_t black_pt_idx = (black_virt >> 12) & 0x3FF;
    uint32_t *black_pd = BLACK_GET_PD();
    if (!(black_pd[black_pd_idx] & PAGE_PRESENT)) return 0;
    uint32_t *black_pt = BLACK_GET_PT(black_pd_idx);
    if (!(black_pt[black_pt_idx] & PAGE_PRESENT)) return 0;
    return (black_pt[black_pt_idx] & 0xFFFFF000) | (black_virt & 0xFFF);
}

int black_vmm_is_mapped(uint32_t black_virt)
{
    uint32_t black_pd_idx = (black_virt >> 22) & 0x3FF;
    uint32_t black_pt_idx = (black_virt >> 12) & 0x3FF;
    uint32_t *black_pd = BLACK_GET_PD();
    if (!(black_pd[black_pd_idx] & PAGE_PRESENT)) return 0;
    uint32_t *black_pt = BLACK_GET_PT(black_pd_idx);
    return (black_pt[black_pt_idx] & PAGE_PRESENT) != 0;
}

void black_vmm_map_range(uint32_t black_virt, uint32_t black_phys,
                          uint32_t black_size, uint32_t black_flags)
{
    for (uint32_t black_off = 0; black_off < black_size; black_off += PAGE_SIZE) {
        black_vmm_map_page(black_virt + black_off, black_phys + black_off, black_flags);
    }
}

int black_vmm_handle_page_fault(uint32_t black_addr, uint32_t black_err)
{
    uint32_t black_pd_idx = (black_addr >> 22) & 0x3FF;
    uint32_t black_pt_idx = (black_addr >> 12) & 0x3FF;
    uint32_t *black_pd = BLACK_GET_PD();

    if (!(black_pd[black_pd_idx] & PAGE_PRESENT)) return -1;

    uint32_t *black_pt = BLACK_GET_PT(black_pd_idx);
    uint32_t black_entry = black_pt[black_pt_idx];

    if ((black_entry & PAGE_COW) && (black_err & 0x02)) {
        uint32_t black_old_phys = black_entry & 0xFFFFF000;
        uint32_t black_frame_idx = black_old_phys / PAGE_SIZE;

        if (black_cow_refcount[black_frame_idx] <= 1) {
            black_pt[black_pt_idx] = (black_old_phys | PAGE_PRESENT | PAGE_WRITE | PAGE_USER) & ~PAGE_COW;
            black_vmm_invlpg(black_addr);
            return 0;
        }

        uint32_t black_new_phys = black_pmm_alloc_frame();
        if (!black_new_phys) return -1;

        uint32_t black_tmp_virt = 0xFFFFF000 - PAGE_SIZE;
        black_vmm_map_page(black_tmp_virt, black_new_phys, PAGE_PRESENT | PAGE_WRITE);
        memcpy((void *)black_tmp_virt, (void *)(black_addr & 0xFFFFF000), PAGE_SIZE);
        black_vmm_unmap_page(black_tmp_virt);

        black_cow_refcount[black_frame_idx]--;
        black_cow_refcount[black_new_phys / PAGE_SIZE] = 1;

        black_pt[black_pt_idx] = black_new_phys | PAGE_PRESENT | PAGE_WRITE | PAGE_USER;
        black_vmm_invlpg(black_addr);
        return 0;
    }

    return -1;
}

void black_vmm_flush_tlb(void)
{
    __asm__ volatile("mov %%cr3, %%eax\nmov %%eax, %%cr3\n" : : : "eax");
}

void black_vmm_invlpg(uint32_t black_virt)
{
    __asm__ volatile("invlpg (%0)" : : "r"(black_virt) : "memory");
}

uint32_t *black_vmm_get_current_pagedir(void) { return BLACK_GET_PD(); }
