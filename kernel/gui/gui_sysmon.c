#include <kernel/kernel.h>
#include <drivers/framebuffer.h>
#include <mm/pmm.h>
#include <mm/heap.h>

void black_gui_sysmon_init(void) {
}

void black_gui_sysmon_render(int black_x, int black_y) {
    uint32_t black_total = black_pmm_get_total_memory() / (1024 * 1024);
    uint32_t black_free = black_pmm_get_free_memory() / (1024 * 1024);
    uint32_t black_used = black_total - black_free;
    (void)black_used;
    black_fb_drawstring(black_x, black_y, "System Monitor", 0x00FF00, 0x000000);
    black_fb_drawstring(black_x, black_y + 16, "CPU: Active", 0x00CC00, 0x000000);
    black_fb_drawstring(black_x, black_y + 32, "MEM: Online", 0x00CC00, 0x000000);
}
