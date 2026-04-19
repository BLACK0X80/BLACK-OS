#ifndef _BLACK_GUI_H
#define _BLACK_GUI_H
void black_gui_wm_init(void);
int black_gui_wm_create(const char *black_title, int black_x, int black_y, int black_w, int black_h);
void black_gui_wm_render(void);
void black_gui_wm_close(int black_id);
void black_gui_desktop_init(void);
void black_gui_desktop_render(void);
void black_gui_notify_init(void);
void black_gui_notify_show(const char *black_msg, uint32_t black_duration);
void black_gui_sysmon_render(int black_x, int black_y);
void black_gui_wallpaper_render(void);
#endif
