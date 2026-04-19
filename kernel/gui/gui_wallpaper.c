#include <kernel/kernel.h>
#include <drivers/framebuffer.h>

void black_gui_wallpaper_init(void) {
}

void black_gui_wallpaper_render(void) {
    for (int black_y = 0; black_y < 768; black_y++) {
        for (int black_x = 0; black_x < 1024; black_x++) {
            uint32_t black_shade = ((black_x ^ black_y) * 3) & 0x1F;
            black_fb_putpixel(black_x, black_y, black_shade << 8);
        }
    }
}
