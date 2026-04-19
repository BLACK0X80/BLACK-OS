#include <kernel/kernel.h>
#include <drivers/framebuffer.h>

static const uint8_t black_icon_folder[] = {
    0x00, 0x7E, 0x42, 0x42, 0xFF, 0x81, 0x81, 0xFF
};

static const uint8_t black_icon_file[] = {
    0x7C, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x7C
};

void black_gui_draw_icon_folder(int black_x, int black_y, uint32_t black_color) {
    for (int black_row = 0; black_row < 8; black_row++) {
        for (int black_col = 0; black_col < 8; black_col++) {
            if (black_icon_folder[black_row] & (1 << (7 - black_col))) {
                black_fb_putpixel(black_x + black_col, black_y + black_row, black_color);
            }
        }
    }
}

void black_gui_draw_icon_file(int black_x, int black_y, uint32_t black_color) {
    for (int black_row = 0; black_row < 8; black_row++) {
        for (int black_col = 0; black_col < 8; black_col++) {
            if (black_icon_file[black_row] & (1 << (7 - black_col))) {
                black_fb_putpixel(black_x + black_col, black_y + black_row, black_color);
            }
        }
    }
}
