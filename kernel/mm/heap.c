#include <mm/heap.h>
#include <mm/vmm.h>
#include <mm/pmm.h>
#include <kernel/kernel.h>
#include <string.h>

typedef struct black_heap_block {
    uint32_t black_magic;
    uint32_t black_size;
    int      black_free;
    struct black_heap_block *black_next;
    struct black_heap_block *black_prev;
} black_heap_block_t;

#define BLACK_HEAP_MAGIC      0xB1AC4E4D
#define BLACK_HEAP_DEAD       0xDEADB10C
#define BLACK_HEAP_MIN_BLOCK  32

static black_heap_block_t *black_heap_head = NULL;
static uint32_t black_heap_start = 0;
static uint32_t black_heap_end = 0;
static uint32_t black_heap_max = 0;

void black_heap_init(uint32_t black_start, uint32_t black_size)
{
    black_heap_start = black_start;
    black_heap_end = black_start + black_size;
    black_heap_max = black_heap_end;

    for (uint32_t black_addr = black_start; black_addr < black_heap_end; black_addr += PAGE_SIZE) {
        uint32_t black_phys = black_pmm_alloc_frame();
        if (black_phys)
            black_vmm_map_page(black_addr, black_phys, PAGE_PRESENT | PAGE_WRITE);
    }

    black_heap_head = (black_heap_block_t *)black_start;
    black_heap_head->black_magic = BLACK_HEAP_MAGIC;
    black_heap_head->black_size = black_size - sizeof(black_heap_block_t);
    black_heap_head->black_free = 1;
    black_heap_head->black_next = NULL;
    black_heap_head->black_prev = NULL;
}

static void black_heap_split(black_heap_block_t *black_block, uint32_t black_size)
{
    if (black_block->black_size <= black_size + sizeof(black_heap_block_t) + BLACK_HEAP_MIN_BLOCK)
        return;

    black_heap_block_t *black_new = (black_heap_block_t *)
        ((uint32_t)black_block + sizeof(black_heap_block_t) + black_size);
    black_new->black_magic = BLACK_HEAP_MAGIC;
    black_new->black_size = black_block->black_size - black_size - sizeof(black_heap_block_t);
    black_new->black_free = 1;
    black_new->black_next = black_block->black_next;
    black_new->black_prev = black_block;

    if (black_block->black_next)
        black_block->black_next->black_prev = black_new;
    black_block->black_next = black_new;
    black_block->black_size = black_size;
}

static void black_heap_merge(black_heap_block_t *black_block)
{
    if (black_block->black_next &&
        black_block->black_next->black_free &&
        black_block->black_next->black_magic == BLACK_HEAP_MAGIC) {
        black_block->black_size += sizeof(black_heap_block_t) + black_block->black_next->black_size;
        black_block->black_next->black_magic = BLACK_HEAP_DEAD;
        black_block->black_next = black_block->black_next->black_next;
        if (black_block->black_next)
            black_block->black_next->black_prev = black_block;
    }
}

void *black_heap_alloc(size_t black_size)
{
    if (black_size == 0) return NULL;
    black_size = BLACK_ALIGN(black_size, 16);

    black_heap_block_t *black_best = NULL;
    black_heap_block_t *black_cur = black_heap_head;

    while (black_cur) {
        if (black_cur->black_magic != BLACK_HEAP_MAGIC) break;
        if (black_cur->black_free && black_cur->black_size >= black_size) {
            if (!black_best || black_cur->black_size < black_best->black_size)
                black_best = black_cur;
            if (black_cur->black_size == black_size) break;
        }
        black_cur = black_cur->black_next;
    }

    if (!black_best) return NULL;

    black_heap_split(black_best, black_size);
    black_best->black_free = 0;
    return (void *)((uint32_t)black_best + sizeof(black_heap_block_t));
}

void black_heap_free(void *black_ptr)
{
    if (!black_ptr) return;

    black_heap_block_t *black_block = (black_heap_block_t *)
        ((uint32_t)black_ptr - sizeof(black_heap_block_t));

    if (black_block->black_magic != BLACK_HEAP_MAGIC) return;

    black_block->black_free = 1;
    memset(black_ptr, 0xCC, black_block->black_size);

    if (black_block->black_prev &&
        black_block->black_prev->black_free)
        black_block = black_block->black_prev;

    black_heap_merge(black_block);
    if (black_block->black_next)
        black_heap_merge(black_block);
}

void *black_heap_calloc(size_t black_count, size_t black_size)
{
    size_t black_total = black_count * black_size;
    void *black_ptr = black_heap_alloc(black_total);
    if (black_ptr) memset(black_ptr, 0, black_total);
    return black_ptr;
}

void *black_heap_realloc(void *black_ptr, size_t black_size)
{
    if (!black_ptr) return black_heap_alloc(black_size);
    if (black_size == 0) { black_heap_free(black_ptr); return NULL; }

    black_heap_block_t *black_block = (black_heap_block_t *)
        ((uint32_t)black_ptr - sizeof(black_heap_block_t));
    if (black_block->black_size >= black_size) return black_ptr;

    void *black_new = black_heap_alloc(black_size);
    if (!black_new) return NULL;
    memcpy(black_new, black_ptr, black_block->black_size);
    black_heap_free(black_ptr);
    return black_new;
}

void *kmalloc(size_t black_size) { return black_heap_alloc(black_size); }
void  kfree(void *black_ptr) { black_heap_free(black_ptr); }
void *kcalloc(size_t black_count, size_t black_size) { return black_heap_calloc(black_count, black_size); }
void *krealloc(void *black_ptr, size_t black_size) { return black_heap_realloc(black_ptr, black_size); }

uint32_t black_heap_get_used(void)
{
    uint32_t black_used = 0;
    black_heap_block_t *black_cur = black_heap_head;
    while (black_cur && black_cur->black_magic == BLACK_HEAP_MAGIC) {
        if (!black_cur->black_free)
            black_used += black_cur->black_size + sizeof(black_heap_block_t);
        black_cur = black_cur->black_next;
    }
    return black_used;
}

uint32_t black_heap_get_free(void)
{
    uint32_t black_free_mem = 0;
    black_heap_block_t *black_cur = black_heap_head;
    while (black_cur && black_cur->black_magic == BLACK_HEAP_MAGIC) {
        if (black_cur->black_free)
            black_free_mem += black_cur->black_size;
        black_cur = black_cur->black_next;
    }
    return black_free_mem;
}
