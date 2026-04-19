#include <kernel/kernel.h>
#include <mm/vmm.h>
#include <mm/pmm.h>
#include <mm/heap.h>

void *black_mmap(uint32_t black_addr, uint32_t black_size, uint32_t black_flags) {
    uint32_t black_pages = (black_size + 0xFFF) / 0x1000;
    uint32_t black_virt = black_addr ? black_addr : 0xB0000000;
    for (uint32_t black_i = 0; black_i < black_pages; black_i++) {
        uint32_t black_phys = black_pmm_alloc_frame();
        black_vmm_map_page(black_virt + black_i * 0x1000, black_phys, black_flags | PAGE_PRESENT | PAGE_WRITE);
    }
    return (void *)black_virt;
}

int black_munmap(uint32_t black_addr, uint32_t black_size) {
    uint32_t black_pages = (black_size + 0xFFF) / 0x1000;
    for (uint32_t black_i = 0; black_i < black_pages; black_i++) {
        black_vmm_unmap_page(black_addr + black_i * 0x1000);
    }
    return 0;
}
