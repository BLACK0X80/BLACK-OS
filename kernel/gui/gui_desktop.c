#include <kernel/kernel.h>
#include <drivers/framebuffer.h>

void black_gui_desktop_init(void) {
}

void black_gui_desktop_render(void) {
    for (int black_x = 0; black_x < 1024; black_x++) {
        for (int black_y = 748; black_y < 768; black_y++) {
            black_fb_putpixel(black_x, black_y, 0x002200);
        }
    }
    black_fb_drawstring(4, 752, "BlackOS PHANTOM", 0x00FF00, 0x002200);
}
