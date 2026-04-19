#include <mm/pmm.h>
#include <kernel/kernel.h>
#include <drivers/serial.h>
#include <string.h>

#define BLACK_MAX_FRAMES  (256 * 1024)
#define BLACK_BITMAP_SIZE (BLACK_MAX_FRAMES / 32)

static uint32_t black_pmm_bitmap[BLACK_BITMAP_SIZE];
static uint32_t black_pmm_total_frames = 0;
static uint32_t black_pmm_used_frames = 0;
static uint32_t black_pmm_total_memory = 0;

extern uint32_t _black_kernel_end_phys;

static inline void black_pmm_set_frame(uint32_t black_frame)
{
    black_pmm_bitmap[black_frame / 32] |= (1 << (black_frame % 32));
}

static inline void black_pmm_clear_frame(uint32_t black_frame)
{
    black_pmm_bitmap[black_frame / 32] &= ~(1 << (black_frame % 32));
}

static inline int black_pmm_test_frame(uint32_t black_frame)
{
    return black_pmm_bitmap[black_frame / 32] & (1 << (black_frame % 32));
}

static uint32_t black_pmm_find_free(void)
{
    for (uint32_t black_i = 0; black_i < BLACK_BITMAP_SIZE; black_i++) {
        if (black_pmm_bitmap[black_i] != 0xFFFFFFFF) {
            for (int black_j = 0; black_j < 32; black_j++) {
                if (!(black_pmm_bitmap[black_i] & (1 << black_j))) {
                    return black_i * 32 + black_j;
                }
            }
        }
    }
    return 0xFFFFFFFF;
}

static uint32_t black_pmm_find_free_contiguous(uint32_t black_count)
{
    uint32_t black_start = 0;
    uint32_t black_found = 0;

    for (uint32_t black_i = 0; black_i < black_pmm_total_frames; black_i++) {
        if (!black_pmm_test_frame(black_i)) {
            if (black_found == 0) black_start = black_i;
            black_found++;
            if (black_found >= black_count) return black_start;
        } else {
            black_found = 0;
        }
    }
    return 0xFFFFFFFF;
}

void black_pmm_init(black_multiboot_info_t *black_mboot)
{
    memset(black_pmm_bitmap, 0xFF, sizeof(black_pmm_bitmap));

    if (!(black_mboot->black_flags & 0x40)) {
        black_pmm_total_memory = (black_mboot->black_mem_upper + 1024) * 1024;
        black_pmm_total_frames = black_pmm_total_memory / BLACK_PAGE_SIZE;

        uint32_t black_kernel_end = (uint32_t)&_black_kernel_end_phys;
        black_kernel_end = BLACK_ALIGN(black_kernel_end, BLACK_PAGE_SIZE);

        for (uint32_t black_addr = black_kernel_end;
             black_addr < black_pmm_total_memory; black_addr += BLACK_PAGE_SIZE) {
            uint32_t black_frame = black_addr / BLACK_PAGE_SIZE;
            if (black_frame < BLACK_MAX_FRAMES)
                black_pmm_clear_frame(black_frame);
        }
        black_pmm_used_frames = black_kernel_end / BLACK_PAGE_SIZE;
        return;
    }

    uint32_t black_mmap_addr = black_mboot->black_mmap_addr + KERNEL_VMA;
    uint32_t black_mmap_end = black_mmap_addr + black_mboot->black_mmap_length;

    while (black_mmap_addr < black_mmap_end) {
        black_mmap_entry_t *black_entry = (black_mmap_entry_t *)black_mmap_addr;

        if (black_entry->black_type == BLACK_MMAP_AVAILABLE) {
            uint64_t black_base = black_entry->black_addr;
            uint64_t black_len = black_entry->black_len;
            black_pmm_total_memory += (uint32_t)black_len;

            for (uint64_t black_addr = black_base;
                 black_addr < black_base + black_len; black_addr += BLACK_PAGE_SIZE) {
                uint32_t black_frame = (uint32_t)(black_addr / BLACK_PAGE_SIZE);
                if (black_frame > 0 && black_frame < BLACK_MAX_FRAMES)
                    black_pmm_clear_frame(black_frame);
            }
        }

        black_mmap_addr += black_entry->black_size + 4;
    }

    uint32_t black_kernel_end = (uint32_t)&_black_kernel_end_phys;
    black_kernel_end = BLACK_ALIGN(black_kernel_end, BLACK_PAGE_SIZE);

    for (uint32_t black_addr = 0; black_addr < black_kernel_end; black_addr += BLACK_PAGE_SIZE) {
        black_pmm_set_frame(black_addr / BLACK_PAGE_SIZE);
    }

    black_pmm_total_frames = black_pmm_total_memory / BLACK_PAGE_SIZE;
    if (black_pmm_total_frames > BLACK_MAX_FRAMES)
        black_pmm_total_frames = BLACK_MAX_FRAMES;

    black_pmm_used_frames = 0;
    for (uint32_t black_i = 0; black_i < black_pmm_total_frames / 32; black_i++) {
        for (int black_j = 0; black_j < 32; black_j++) {
            if (black_pmm_bitmap[black_i] & (1 << black_j))
                black_pmm_used_frames++;
        }
    }
}

uint32_t black_pmm_alloc_frame(void)
{
    uint32_t black_frame = black_pmm_find_free();
    if (black_frame == 0xFFFFFFFF) return 0;
    black_pmm_set_frame(black_frame);
    black_pmm_used_frames++;
    return black_frame * BLACK_PAGE_SIZE;
}

void black_pmm_free_frame(uint32_t black_addr)
{
    uint32_t black_frame = black_addr / BLACK_PAGE_SIZE;
    if (black_frame >= BLACK_MAX_FRAMES) return;
    if (black_pmm_test_frame(black_frame)) {
        black_pmm_clear_frame(black_frame);
        black_pmm_used_frames--;
    }
}

uint32_t black_pmm_alloc_frames(uint32_t black_count)
{
    uint32_t black_start = black_pmm_find_free_contiguous(black_count);
    if (black_start == 0xFFFFFFFF) return 0;
    for (uint32_t black_i = 0; black_i < black_count; black_i++) {
        black_pmm_set_frame(black_start + black_i);
        black_pmm_used_frames++;
    }
    return black_start * BLACK_PAGE_SIZE;
}

void black_pmm_free_frames(uint32_t black_addr, uint32_t black_count)
{
    uint32_t black_frame = black_addr / BLACK_PAGE_SIZE;
    for (uint32_t black_i = 0; black_i < black_count; black_i++) {
        if (black_frame + black_i < BLACK_MAX_FRAMES) {
            black_pmm_clear_frame(black_frame + black_i);
            black_pmm_used_frames--;
        }
    }
}

uint32_t black_pmm_get_total_memory(void) { return black_pmm_total_memory; }
uint32_t black_pmm_get_free_memory(void) { return (black_pmm_total_frames - black_pmm_used_frames) * BLACK_PAGE_SIZE; }
uint32_t black_pmm_get_used_memory(void) { return black_pmm_used_frames * BLACK_PAGE_SIZE; }
