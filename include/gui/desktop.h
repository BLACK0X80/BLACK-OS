#ifndef _BLACK_GUI_DESKTOP_H
#define _BLACK_GUI_DESKTOP_H

#include <stdint.h>

void black_gui_init(void *black_mboot_info);
int black_gui_create_window(const char *black_title, int black_x, int black_y, int black_w, int black_h);
void black_gui_update_boot_progress(int black_percent, const char *black_msg);
void black_gui_loop(void);

#endif
