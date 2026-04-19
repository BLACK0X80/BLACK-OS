#include <kernel/kernel.h>
#include <kernel/multiboot.h>
#include <drivers/framebuffer.h>
#include <drivers/mouse.h>
#include <drivers/keyboard.h>
#include <drivers/pit.h>
#include <mm/heap.h>
#include <mm/pmm.h>
#include <kernel/scheduler.h>
#include <fs/vfs.h>
#include <string.h>

#define BLACK_GUI_COLOR_BG       0x000000
#define BLACK_GUI_COLOR_FG       0x00FF00
#define BLACK_GUI_COLOR_DIM      0x003300
#define BLACK_GUI_COLOR_BRIGHT   0x00FF41
#define BLACK_GUI_COLOR_TASKBAR  0x001100

static uint32_t black_theme_color = 0x00AA00;

#define BLACK_MAX_WINDOWS  16
#define BLACK_WIN_TITLEBAR 24

#define BLACK_TERM_COLS    72
#define BLACK_TERM_ROWS    45
#define BLACK_TERM_BUF     (BLACK_TERM_COLS * BLACK_TERM_ROWS)

#define BLACK_MENU_NONE    0
#define BLACK_MENU_START   1
#define BLACK_MENU_CONTEXT 2

typedef struct {
    int black_x, black_y;
    char black_label[32];
    int black_command_id;
} black_desktop_icon_t;

static black_desktop_icon_t black_dicons[4] = {
    {30,  30, "My OS", 3},
    {30, 110, "Terminal", 1},
    {30, 190, "System", 2},
    {30, 270, "Settings", 4}
};

typedef struct {
    int black_x, black_y, black_w, black_h;
    char black_title[64];
    int black_active;
    int black_minimized;
    int black_maximized;
    int black_saved_x, black_saved_y, black_saved_w, black_saved_h;
    char black_termbuf[BLACK_TERM_BUF];
    int black_term_cx;
    int black_term_cy;
    char black_term_input[256];
    int black_term_input_pos;
    int black_term_has_prompt;
    char black_vfs_path[256];
} black_window_t;

static black_window_t black_windows[BLACK_MAX_WINDOWS];
static int black_win_order[BLACK_MAX_WINDOWS];
static int black_win_count = 0;
static int black_gui_running = 0;
static int black_gui_needs_redraw = 1;

static int black_menu_state = BLACK_MENU_NONE;
static int black_menu_x = 0;
static int black_menu_y = 0;
static int black_menu_hover = -1;

typedef struct {
    char black_label[32];
    int black_id;
} black_menu_item_t;

static black_menu_item_t black_start_items[] = {
    {"  Terminal", 1},
    {"  System Monitor", 2},
    {"  File Manager", 3},
    {"  Settings Themes", 4},
    {"  Lock System", 5},
    {"  Shutdown", 6}
};
static int black_start_items_count = 6;

static black_menu_item_t black_ctx_items[] = {
    {"  New Text Note", 1},
    {"  New Folder", 2},
    {"  Terminal", 3},
    {"  Tasks View", 4}
};
static int black_ctx_items_count = 4;

static int black_drag_win = -1;
static int black_drag_ox = 0;
static int black_drag_oy = 0;

static int black_resize_win = -1;
static int black_resize_ox = 0;
static int black_resize_oy = 0;
static int black_resize_ow = 0;
static int black_resize_oh = 0;
static int black_resize_edge = 0;

#define BLACK_EDGE_R 2
#define BLACK_EDGE_B 4
#define BLACK_EDGE_BR 8

static uint8_t black_prev_buttons = 0;
static uint64_t black_last_click_time = 0;

static int black_point_in_rect(int px, int py, int rx, int ry, int rw, int rh)
{
    return (px >= rx && px < rx + rw && py >= ry && py < ry + rh);
}

static void black_gui_bring_to_front(int black_wid)
{
    int black_idx = -1;
    for (int i = 0; i < black_win_count; i++) {
        if (black_win_order[i] == black_wid) { black_idx = i; break; }
    }
    if (black_idx == -1 || black_idx == black_win_count - 1) return;
    for (int i = black_idx; i < black_win_count - 1; i++) {
        black_win_order[i] = black_win_order[i + 1];
    }
    black_win_order[black_win_count - 1] = black_wid;
    black_gui_needs_redraw = 1;
}

static int black_gui_get_focused(void)
{
    if (black_win_count == 0) return -1;
    return black_win_order[black_win_count - 1];
}

extern void black_execute_cmd(const char *);

int black_gui_create_window(const char *black_title, int black_x, int black_y, int black_w, int black_h)
{
    if (black_win_count >= BLACK_MAX_WINDOWS) return -1;
    int black_idx = -1;
    for (int i = 0; i < BLACK_MAX_WINDOWS; i++) {
        if (!black_windows[i].black_active) { black_idx = i; break; }
    }
    if (black_idx == -1) return -1;
    
    black_window_t *black_win = &black_windows[black_idx];
    memset(black_win, 0, sizeof(black_window_t));
    strncpy(black_win->black_title, black_title, 63);
    black_win->black_x = black_x;
    black_win->black_y = black_y;
    black_win->black_w = black_w;
    black_win->black_h = black_h;
    black_win->black_active = 1;
    black_win->black_term_cx = 0;
    black_win->black_term_cy = 0;
    black_win->black_term_input_pos = 0;
    black_win->black_term_has_prompt = 0;
    strcpy(black_win->black_vfs_path, "/");
    
    black_win_order[black_win_count++] = black_idx;
    black_gui_needs_redraw = 1;
    return black_idx;
}

static void black_gui_destroy_window(int black_wid)
{
    if (black_wid < 0 || black_wid >= BLACK_MAX_WINDOWS || !black_windows[black_wid].black_active) return;
    black_windows[black_wid].black_active = 0;
    int black_idx = -1;
    for (int i = 0; i < black_win_count; i++) {
        if (black_win_order[i] == black_wid) { black_idx = i; break; }
    }
    if (black_idx != -1) {
        for (int i = black_idx; i < black_win_count - 1; i++) {
            black_win_order[i] = black_win_order[i + 1];
        }
        black_win_count--;
    }
    black_gui_needs_redraw = 1;
}

static void black_term_scroll(black_window_t *black_w)
{
    memmove(black_w->black_termbuf, black_w->black_termbuf + BLACK_TERM_COLS, BLACK_TERM_COLS * (BLACK_TERM_ROWS - 1));
    memset(black_w->black_termbuf + BLACK_TERM_COLS * (BLACK_TERM_ROWS - 1), 0, BLACK_TERM_COLS);
    black_w->black_term_cy--;
}

static void black_term_putchar(black_window_t *black_w, char black_c)
{
    if (black_c == '\n') {
        black_w->black_term_cx = 0;
        black_w->black_term_cy++;
        if (black_w->black_term_cy >= BLACK_TERM_ROWS) black_term_scroll(black_w);
        return;
    }
    if (black_c == '\b') {
        if (black_w->black_term_cx > 0) {
            black_w->black_term_cx--;
            black_w->black_termbuf[black_w->black_term_cy * BLACK_TERM_COLS + black_w->black_term_cx] = 0;
        }
        return;
    }
    if (black_w->black_term_cx >= BLACK_TERM_COLS) {
        black_w->black_term_cx = 0;
        black_w->black_term_cy++;
        if (black_w->black_term_cy >= BLACK_TERM_ROWS) black_term_scroll(black_w);
    }
    black_w->black_termbuf[black_w->black_term_cy * BLACK_TERM_COLS + black_w->black_term_cx] = black_c;
    black_w->black_term_cx++;
}

static void black_term_puts(black_window_t *black_w, const char *black_s)
{
    while (*black_s) black_term_putchar(black_w, *black_s++);
}

static void black_term_prompt(black_window_t *black_w)
{
    black_term_puts(black_w, "black@phantom:/$ ");
    black_w->black_term_input_pos = 0;
    black_w->black_term_has_prompt = 1;
}

static void black_term_exec(black_window_t *black_w, const char *black_cmd)
{
    if (black_cmd[0] == '\0') return;
    extern void black_vga_set_callback(void (*cb)(char));
    static black_window_t *black_current_w = NULL;
    black_current_w = black_w;
    void black_term_vga_cb(char c) { if (black_current_w) black_term_putchar(black_current_w, c); }
    black_vga_set_callback(black_term_vga_cb);
    black_execute_cmd(black_cmd);
    black_vga_set_callback(NULL);
    black_current_w = NULL;
}

void black_gui_key_input(char black_c)
{
    int black_fid = black_gui_get_focused();
    if (black_fid < 0) return;
    black_window_t *black_w = &black_windows[black_fid];
    black_gui_needs_redraw = 1;
    if (strcmp(black_w->black_title, "System Monitor") == 0 || strcmp(black_w->black_title, "File Manager") == 0 || strcmp(black_w->black_title, "Settings") == 0) return;

    int black_is_notepad = (strcmp(black_w->black_title, "Notepad") == 0);

    if (!black_is_notepad && !black_w->black_term_has_prompt) {
        black_term_puts(black_w, "Welcome to BlackOS PHANTOM Terminal\n");
        black_term_prompt(black_w);
    }
    
    if (black_is_notepad && !black_w->black_term_has_prompt) {
        black_w->black_term_has_prompt = 1;
    }

    if (black_c == '\n') {
        black_term_putchar(black_w, '\n');
        if (!black_is_notepad) {
            black_w->black_term_input[black_w->black_term_input_pos] = 0;
            black_term_exec(black_w, black_w->black_term_input);
            black_w->black_term_input_pos = 0;
            black_term_prompt(black_w);
        }
    } else if (black_c == '\b') {
        if (!black_is_notepad && black_w->black_term_input_pos > 0) {
            black_w->black_term_input_pos--;
            black_term_putchar(black_w, '\b');
        } else if (black_is_notepad) {
            black_term_putchar(black_w, '\b');
        }
    } else if (black_c >= 32 && black_c < 127) {
        if (!black_is_notepad && black_w->black_term_input_pos < 255) black_w->black_term_input[black_w->black_term_input_pos++] = black_c;
        black_term_putchar(black_w, black_c);
    }
}

static void black_gui_draw_menu(black_menu_item_t *black_items, int black_count, int black_mx, int black_my)
{
    int black_mw = 180;
    int black_mh = black_count * 28 + 10;
    black_fb_fillrect(black_mx, black_my, black_mw, black_mh, 0x000A0A);
    black_fb_fillrect(black_mx, black_my, black_mw, 1, black_theme_color);
    black_fb_fillrect(black_mx, black_my + black_mh - 1, black_mw, 1, black_theme_color);
    black_fb_fillrect(black_mx, black_my, 1, black_mh, black_theme_color);
    black_fb_fillrect(black_mx + black_mw - 1, black_my, 1, black_mh, black_theme_color);

    for (int i = 0; i < black_count; i++) {
        int black_iy = black_my + 5 + i * 28;
        if (i == black_menu_hover) {
            black_fb_fillrect(black_mx + 2, black_iy, black_mw - 4, 28, black_theme_color);
        }
        black_fb_drawstring(black_mx + 10, black_iy + 10, black_items[i].black_label, BLACK_GUI_COLOR_BRIGHT, 
            (i == black_menu_hover) ? black_theme_color : 0x000A0A);
    }
}

static void black_gui_draw_taskbar(void)
{
    black_framebuffer_t *black_fb = black_fb_get();
    uint32_t black_tb_y = black_fb->black_height - 30;
    black_fb_fillrect(0, black_tb_y, black_fb->black_width, 30, BLACK_GUI_COLOR_TASKBAR);
    black_fb_fillrect(0, black_tb_y, black_fb->black_width, 2, black_theme_color);
    
    uint32_t black_start_color = (black_menu_state == BLACK_MENU_START) ? black_theme_color : BLACK_GUI_COLOR_DIM;
    black_fb_fillrect(4, black_tb_y + 4, 80, 22, black_start_color);
    black_fb_drawstring(12, black_tb_y + 10, "[ BLACK ]", 0xFFFFFF, black_start_color);

    int black_btn_w = 120;
    int black_bx = 96;
    int black_focused_id = black_gui_get_focused();
    for (int i = 0; i < black_win_count; i++) {
        int black_wid = black_win_order[i];
        black_window_t *black_w = &black_windows[black_wid];
        uint32_t black_bc = (black_wid == black_focused_id && !black_w->black_minimized) ? 0x004400 : 0x001100;
        black_fb_fillrect(black_bx, black_tb_y + 4, black_btn_w, 22, black_bc);
        black_fb_fillrect(black_bx, black_tb_y + 4, black_btn_w, 1, black_theme_color);
        black_fb_fillrect(black_bx, black_tb_y + 25, black_btn_w, 1, black_theme_color);
        black_fb_fillrect(black_bx, black_tb_y + 4, 1, 22, black_theme_color);
        black_fb_fillrect(black_bx + black_btn_w - 1, black_tb_y + 4, 1, 22, black_theme_color);
        char black_tbuf[16];
        strncpy(black_tbuf, black_w->black_title, 14);
        black_tbuf[14] = 0;
        black_fb_drawstring(black_bx + 10, black_tb_y + 10, black_tbuf, 0xFFFFFF, black_bc);
        black_bx += black_btn_w + 4;
    }

    char black_time[16];
    uint64_t black_ms = black_pit_get_uptime_ms();
    uint32_t black_s = (uint32_t)(black_ms / 1000);
    uint32_t black_m = black_s / 60; black_s %= 60;
    uint32_t black_hr = black_m / 60; black_m %= 60;
    black_time[0] = '0' + (black_hr / 10); black_time[1] = '0' + (black_hr % 10);
    black_time[2] = ':';
    black_time[3] = '0' + (black_m / 10); black_time[4] = '0' + (black_m % 10);
    black_time[5] = ':';
    black_time[6] = '0' + (black_s / 10); black_time[7] = '0' + (black_s % 10);
    black_time[8] = 0;
    black_fb_drawstring(black_fb->black_width - 80, black_tb_y + 10, black_time, 0xFFFFFF, BLACK_GUI_COLOR_TASKBAR);
    black_fb_fillrect(black_fb->black_width - 90, black_tb_y, 1, 30, black_theme_color);
}

static void black_gui_draw_window(black_window_t *black_w, int black_is_focused)
{
    if (!black_w->black_active || black_w->black_minimized) return;
    
    black_fb_fillrect(black_w->black_x + 5, black_w->black_y + 5, black_w->black_w, black_w->black_h, 0x000707);

    uint32_t black_tcol = black_is_focused ? black_theme_color : 0x002200;
    black_fb_fillrect(black_w->black_x, black_w->black_y, black_w->black_w, BLACK_WIN_TITLEBAR, black_tcol);
    black_fb_fillrect(black_w->black_x, black_w->black_y + BLACK_WIN_TITLEBAR - 2, black_w->black_w, 2, 0xFFFFFF);
    black_fb_drawstring(black_w->black_x + 10, black_w->black_y + 8, black_w->black_title, 0xFFFFFF, black_tcol);
    
    int black_btn_x = black_w->black_x + black_w->black_w - 24;
    black_fb_fillrect(black_btn_x, black_w->black_y + 4, 16, 16, 0xAA0000);
    black_fb_drawchar(black_btn_x + 4, black_w->black_y + 8, 'X', 0xFFFFFF, 0xAA0000);
    
    int black_max_btn_x = black_btn_x - 20;
    black_fb_fillrect(black_max_btn_x, black_w->black_y + 4, 16, 16, 0x006600);
    black_fb_drawchar(black_max_btn_x + 4, black_w->black_y + 8, '^', 0xFFFFFF, 0x006600);

    int black_min_btn_x = black_max_btn_x - 20;
    black_fb_fillrect(black_min_btn_x, black_w->black_y + 4, 16, 16, 0x000066);
    black_fb_drawchar(black_min_btn_x + 4, black_w->black_y + 8, '_', 0xFFFFFF, 0x000066);

    if (strcmp(black_w->black_title, "Notepad") == 0) {
        int black_save_btn_x = black_min_btn_x - 50;
        black_fb_fillrect(black_save_btn_x, black_w->black_y + 4, 46, 16, 0x333333);
        black_fb_fillrect(black_save_btn_x, black_w->black_y + 4, 1, 16, 0x777777);
        black_fb_drawstring(black_save_btn_x + 5, black_w->black_y + 8, "SAVE", 0xFFFFFF, 0x333333);
    }

    black_fb_fillrect(black_w->black_x, black_w->black_y + BLACK_WIN_TITLEBAR, black_w->black_w, black_w->black_h - BLACK_WIN_TITLEBAR, 0x001111);
    
    if (strcmp(black_w->black_title, "System Monitor") == 0) {
        int black_pad = 10;
        int black_sy = black_w->black_y + BLACK_WIN_TITLEBAR + black_pad;
        int black_sx = black_w->black_x + black_pad;
        
        black_fb_drawstring(black_sx, black_sy, "=== System Monitor (Click Task to Kill) ===", BLACK_GUI_COLOR_BRIGHT, 0x001111);
        black_sy += 20;

        uint32_t black_tot = black_pmm_get_total_memory() / (1024*1024);
        uint32_t black_fre = black_pmm_get_free_memory() / (1024*1024);
        (void)black_fre;
        uint32_t black_use = black_pmm_get_used_memory() / (1024*1024);
        
        black_fb_drawstring(black_sx, black_sy, "RAM Usage:", 0xFFFFFF, 0x001111);
        black_sy += 12;
        int black_bar_w = black_w->black_w - (black_pad * 2);
        black_fb_fillrect(black_sx, black_sy, black_bar_w, 20, 0x003300);
        if (black_tot > 0) {
            int black_fill = (black_use * black_bar_w) / black_tot;
            if (black_fill > black_bar_w) black_fill = black_bar_w;
            black_fb_fillrect(black_sx, black_sy, black_fill, 20, black_theme_color);
        }
        black_fb_fillrect(black_sx, black_sy, black_bar_w, 1, 0xFFFFFF);
        black_fb_fillrect(black_sx, black_sy+19, black_bar_w, 1, 0xFFFFFF);
        black_fb_fillrect(black_sx, black_sy, 1, 20, 0xFFFFFF);
        black_fb_fillrect(black_sx+black_bar_w-1, black_sy, 1, 20, 0xFFFFFF);
        black_sy += 30;

        int black_tmax;
        black_task_t *black_all = black_scheduler_get_tasks(&black_tmax);
        black_fb_drawstring(black_sx, black_sy, "TID   NAME             STATE", black_theme_color, 0x001111);
        black_fb_fillrect(black_sx, black_sy + 10, black_bar_w, 1, black_theme_color);
        black_sy += 16;
        static const char *black_st[] = { "UNU", "RUN", "RDY", "BLK", "SLP", "ZOM" };
        for (int i=0; i<black_tmax && black_sy < black_w->black_y + black_w->black_h - 20; i++) {
            if (black_all[i].black_state == BLACK_TASK_STATE_UNUSED) continue;
            char black_lbuf[64] = {0};
            int black_bi = 0;
            uint32_t black_tid = black_all[i].black_tid;
            if (black_tid==0) black_lbuf[black_bi++] = '0';
            else { char b[10]; int idx=0; while(black_tid>0) {b[idx++]='0'+(black_tid%10); black_tid/=10;} while(idx>0) black_lbuf[black_bi++]=b[--idx]; }
            while(black_bi < 6) black_lbuf[black_bi++] = ' ';
            for(int j=0; j<16; j++) {
                if (black_all[i].black_name[j]) black_lbuf[black_bi++] = black_all[i].black_name[j];
                else break;
            }
            while(black_bi < 23) black_lbuf[black_bi++] = ' ';
            int st_idx = black_all[i].black_state; if (st_idx > 5) st_idx = 0;
            black_lbuf[black_bi++] = black_st[st_idx][0];
            black_lbuf[black_bi++] = black_st[st_idx][1];
            black_lbuf[black_bi++] = black_st[st_idx][2];
            black_lbuf[black_bi] = 0;
            
            uint32_t black_row_color = (black_all[i].black_state == BLACK_TASK_STATE_ZOMBIE) ? 0xAA0000 : 0xFFFFFF;
            black_fb_drawstring(black_sx, black_sy, black_lbuf, black_row_color, 0x001111);
            black_sy += 12;
        }
    } else if (strcmp(black_w->black_title, "File Manager") == 0) {
        int black_fx = black_w->black_x + 10;
        int black_fy = black_w->black_y + BLACK_WIN_TITLEBAR + 10;
        
        char black_abuf[256];
        strcpy(black_abuf, "Path: ");
        strcat(black_abuf, black_w->black_vfs_path);
        black_fb_drawstring(black_fx, black_fy, black_abuf, 0xFFFFFF, 0x001111);
        
        black_fb_fillrect(black_fx, black_fy + 16, black_w->black_w - 20, 1, 0x555555);
        
        int black_up_x = black_w->black_x + black_w->black_w - 70;
        black_fb_fillrect(black_up_x, black_fy - 4, 50, 15, 0x333333);
        black_fb_drawstring(black_up_x + 5, black_fy, "[ UP ]", 0xFFFFFF, 0x333333);
        
        black_fy += 30;

        black_vfs_node_t *black_dir_node = black_vfs_lookup(black_w->black_vfs_path);
        if (black_dir_node && (black_dir_node->black_flags & VFS_DIRECTORY)) {
            int black_idx = 0;
            black_dirent_t *black_d;
            while ((black_d = black_vfs_readdir(black_dir_node, black_idx)) != NULL) {
                int bx = black_fx + (black_idx % 5) * 80;
                int by = black_fy + (black_idx / 5) * 70;
                
                if (black_d->black_type & VFS_DIRECTORY) {
                    black_fb_fillrect(bx, by + 5, 40, 30, 0xCCCC00);
                    black_fb_fillrect(bx, by, 15, 5, 0xCCCC00);
                    black_fb_fillrect(bx + 2, by + 10, 36, 20, 0xFFFF00);
                } else {
                    black_fb_fillrect(bx + 5, by, 30, 35, 0xEEEEEE);
                    black_fb_fillrect(bx + 10, by + 10, 20, 2, 0x333333);
                    black_fb_fillrect(bx + 10, by + 15, 20, 2, 0x333333);
                    black_fb_fillrect(bx + 10, by + 20, 15, 2, 0x333333);
                }
                
                int black_lw = strlen(black_d->black_name) * 8;
                black_fb_drawstring(bx + 20 - (black_lw / 2), by + 45, black_d->black_name, 0xFFFFFF, 0x001111);
                black_idx++;
            }
        } else {
            black_fb_drawstring(black_fx, black_fy, "Directory not found.", 0xFF0000, 0x001111);
        }
    } else if (strcmp(black_w->black_title, "Settings") == 0) {
        int black_sx = black_w->black_x + 10;
        int black_sy = black_w->black_y + BLACK_WIN_TITLEBAR + 20;
        black_fb_drawstring(black_sx, black_sy, "Select System Accent Color:", 0xFFFFFF, 0x001111);
        black_sy += 30;
        black_fb_fillrect(black_sx, black_sy, 50, 50, 0x00AA00);
        black_fb_drawstring(black_sx + 5, black_sy + 20, "Green", 0xFFFFFF, 0x00AA00);
        black_fb_fillrect(black_sx + 60, black_sy, 50, 50, 0x0055FF);
        black_fb_drawstring(black_sx + 65, black_sy + 20, " Blue", 0xFFFFFF, 0x0055FF);
        black_fb_fillrect(black_sx + 120, black_sy, 50, 50, 0xAA0000);
        black_fb_drawstring(black_sx + 125, black_sy + 20, " Red", 0xFFFFFF, 0xAA0000);
        black_fb_fillrect(black_sx + 180, black_sy, 50, 50, 0x990099);
        black_fb_drawstring(black_sx + 185, black_sy + 20, "Prple", 0xFFFFFF, 0x990099);
    } else {
        int black_cx = black_w->black_x + 4;
        int black_cy = black_w->black_y + BLACK_WIN_TITLEBAR + 4;
        int black_max_rows = (black_w->black_h - BLACK_WIN_TITLEBAR - 8) / 8;
        int black_max_cols = (black_w->black_w - 8) / 8;
        if (black_max_rows > BLACK_TERM_ROWS) black_max_rows = BLACK_TERM_ROWS;
        if (black_max_cols > BLACK_TERM_COLS) black_max_cols = BLACK_TERM_COLS;
        for (int black_r = 0; black_r < black_max_rows; black_r++) {
            for (int black_c = 0; black_c < black_max_cols; black_c++) {
                char black_ch = black_w->black_termbuf[black_r * BLACK_TERM_COLS + black_c];
                if (black_ch >= 32 && black_ch < 127)
                    black_fb_drawchar(black_cx + black_c * 8, black_cy + black_r * 8, black_ch, BLACK_GUI_COLOR_FG, 0x001111);
            }
        }
        if (black_is_focused && black_w->black_term_cy < black_max_rows && black_w->black_term_cx < black_max_cols) {
            black_fb_fillrect(black_cx + black_w->black_term_cx * 8, black_cy + black_w->black_term_cy * 8, 8, 8, BLACK_GUI_COLOR_FG);
        }
    }
    
    black_fb_fillrect(black_w->black_x, black_w->black_y, 1, black_w->black_h, black_theme_color);
    black_fb_fillrect(black_w->black_x + black_w->black_w - 1, black_w->black_y, 1, black_w->black_h, black_theme_color);
    black_fb_fillrect(black_w->black_x, black_w->black_y + black_w->black_h - 1, black_w->black_w, 1, black_theme_color);
}

static void black_gui_draw_cursor(int black_mx, int black_my)
{
    for (int i = 0; i < 14; i++) black_fb_putpixel(black_mx, black_my + i, 0xFFFFFF);
    for (int i = 0; i < 10; i++) black_fb_putpixel(black_mx + i, black_my + i, 0xFFFFFF);
    for (int i = 0; i < 5; i++) black_fb_putpixel(black_mx + i, black_my + 9, 0xFFFFFF);
}

static void black_gui_draw_desktop_icons(void)
{
    for (int i = 0; i < 4; i++) {
        int black_ix = black_dicons[i].black_x;
        int black_iy = black_dicons[i].black_y;
        black_fb_fillrect(black_ix, black_iy + 5, 40, 30, 0xCCCC00);
        black_fb_fillrect(black_ix, black_iy, 15, 5, 0xCCCC00);
        black_fb_fillrect(black_ix + 2, black_iy + 10, 36, 20, 0xFFFF00);
        int black_lw = strlen(black_dicons[i].black_label) * 8;
        int black_tx = black_ix + 20 - (black_lw / 2);
        black_fb_drawstring(black_tx, black_iy + 40, black_dicons[i].black_label, 0xFFFFFF, BLACK_GUI_COLOR_BG);
    }
}

static void black_gui_draw_matrix_bg(void)
{
    black_framebuffer_t *black_fb = black_fb_get();
    static uint32_t black_seed = 42;
    for (uint32_t black_col = 0; black_col < black_fb->black_width; black_col += 16) {
        black_seed = black_seed * 1103515245 + 12345;
        if ((black_seed >> 16) % 32 == 0) {
            uint32_t black_len = (black_seed >> 8) % 20 + 5;
            uint32_t black_start = (black_seed >> 4) % (black_fb->black_height / 8);
            for (uint32_t black_r = 0; black_r < black_len; black_r++) {
                black_seed = black_seed * 1103515245 + 12345;
                char black_c = '0' + ((black_seed >> 16) % 10);
                uint32_t black_brightness = (black_r == 0) ? black_theme_color : (black_r < 3) ? (black_theme_color & 0x777777) : (black_theme_color & 0x333333);
                black_fb_drawchar(black_col, (black_start + black_r) * 8, black_c, black_brightness, BLACK_GUI_COLOR_BG);
            }
        }
    }
    black_gui_draw_desktop_icons();
}

static void black_gui_handle_mouse(black_mouse_state_t *black_ms)
{
    black_framebuffer_t *black_fb = black_fb_get();
    uint8_t black_pressed = black_ms->black_buttons & ~black_prev_buttons;
    black_prev_buttons = black_ms->black_buttons;
    int black_mx = black_ms->black_x;
    int black_my = black_ms->black_y;

    if (black_mx != black_drag_ox || black_my != black_drag_oy) black_gui_needs_redraw = 1;

    if (black_menu_state != BLACK_MENU_NONE) {
        int black_mcnt = (black_menu_state == BLACK_MENU_START) ? black_start_items_count : black_ctx_items_count;
        int black_mh = black_mcnt * 28 + 10;
        int black_my_pos = black_my - black_menu_y - 5;
        black_menu_hover = -1;
        if (black_mx >= black_menu_x && black_mx < black_menu_x + 180 && black_my >= black_menu_y && black_my < black_menu_y + black_mh) {
            if (black_my_pos >= 0) black_menu_hover = black_my_pos / 28;
            if (black_menu_hover >= black_mcnt) black_menu_hover = -1;
        }
    }

    if (black_ms->black_buttons & 1) {
        if (black_menu_state != BLACK_MENU_NONE && (black_pressed & 1)) {
            if (black_menu_hover >= 0) {
                if (black_menu_state == BLACK_MENU_START) {
                    if (black_menu_hover == 0) black_gui_create_window("Terminal", 100, 100, 500, 350);
                    else if (black_menu_hover == 1) black_gui_create_window("System Monitor", 150, 150, 400, 500);
                    else if (black_menu_hover == 2) black_gui_create_window("File Manager", 200, 100, 500, 400);
                    else if (black_menu_hover == 3) black_gui_create_window("Settings", 300, 150, 300, 200);
                    else if (black_menu_hover == 4) black_execute_cmd("halt");
                } else if (black_menu_state == BLACK_MENU_CONTEXT) {
                    if (black_menu_hover == 0) black_gui_create_window("Notepad", black_mx, black_my, 400, 300);
                    else if (black_menu_hover == 1) {
                        char black_dbuf[32];
                        uint32_t black_ticks = (uint32_t)black_pit_get_uptime_ms();
                        char b[10]; int idx=0; while(black_ticks>0) {b[idx++]='0'+(black_ticks%10); black_ticks/=10;} 
                        strcpy(black_dbuf, "NewDir_"); int bk=7; while(idx>0) black_dbuf[bk++]=b[--idx]; black_dbuf[bk]=0;
                        black_vfs_mkdir(black_dbuf);
                        black_gui_create_window("File Manager", black_mx, black_my, 500, 400);
                    }
                    else if (black_menu_hover == 2) black_gui_create_window("Terminal", black_mx, black_my, 500, 350);
                    else if (black_menu_hover == 3) black_gui_create_window("System Monitor", black_mx, black_my, 400, 500);
                }
            }
            black_menu_state = BLACK_MENU_NONE;
            black_gui_needs_redraw = 1;
            return;
        }

        if (black_drag_win >= 0) {
            black_windows[black_drag_win].black_x = black_mx - black_drag_ox;
            black_windows[black_drag_win].black_y = black_my - black_drag_oy;
            black_gui_needs_redraw = 1;
        } else if (black_resize_win >= 0) {
            black_window_t *w = &black_windows[black_resize_win];
            int dx = black_mx - black_resize_ox;
            int dy = black_my - black_resize_oy;
            if (black_resize_edge == BLACK_EDGE_R || black_resize_edge == BLACK_EDGE_BR) w->black_w = black_resize_ow + dx;
            if (black_resize_edge == BLACK_EDGE_B || black_resize_edge == BLACK_EDGE_BR) w->black_h = black_resize_oh + dy;
            if (w->black_w < 100) w->black_w = 100;
            if (w->black_h < 50) w->black_h = 50;
            black_gui_needs_redraw = 1;
        } else if (black_pressed & 1) {
            black_menu_state = BLACK_MENU_NONE;
            if (black_my >= (int)black_fb->black_height - 30) {
                if (black_mx < 84) {
                    black_menu_state = BLACK_MENU_START;
                    int black_menu_h = black_start_items_count * 28 + 10;
                    black_menu_x = 0;
                    black_menu_y = black_fb->black_height - 30 - black_menu_h;
                } else {
                    int bx = 96;
                    for (int i = 0; i < black_win_count; i++) {
                        if (black_mx >= bx && black_mx < bx + 120) {
                            int wid = black_win_order[i];
                            if (black_gui_get_focused() == wid && !black_windows[wid].black_minimized) {
                                black_windows[wid].black_minimized = 1;
                            } else {
                                black_windows[wid].black_minimized = 0;
                                black_gui_bring_to_front(wid);
                            }
                            break;
                        }
                        bx += 124;
                    }
                }
                black_gui_needs_redraw = 1;
                return;
            }

            for (int i = 0; i < 4; i++) {
                int bx = black_dicons[i].black_x;
                int by = black_dicons[i].black_y;
                if (black_mx >= bx && black_mx < bx + 40 && black_my >= by && black_my < by + 40) {
                    if (black_dicons[i].black_command_id == 1) black_gui_create_window("Terminal", 100 + i*20, 100 + i*20, 500, 350);
                    else if (black_dicons[i].black_command_id == 2) black_gui_create_window("System Monitor", 150, 150, 400, 500);
                    else if (black_dicons[i].black_command_id == 3) black_gui_create_window("File Manager", 200, 100, 500, 400);
                    else if (black_dicons[i].black_command_id == 4) black_gui_create_window("Settings", 300, 150, 300, 200);
                    black_gui_needs_redraw = 1;
                    return;
                }
            }

            for (int i = black_win_count - 1; i >= 0; i--) {
                int wid = black_win_order[i];
                black_window_t *w = &black_windows[wid];
                if (!w->black_active || w->black_minimized) continue;
                
                if (strcmp(w->black_title, "Settings") == 0) {
                    int black_sx = w->black_x + 10;
                    int black_sy = w->black_y + BLACK_WIN_TITLEBAR + 50;
                    if (black_point_in_rect(black_mx, black_my, black_sx, black_sy, 50, 50)) black_theme_color = 0x00AA00;
                    if (black_point_in_rect(black_mx, black_my, black_sx+60, black_sy, 50, 50)) black_theme_color = 0x0055FF;
                    if (black_point_in_rect(black_mx, black_my, black_sx+120, black_sy, 50, 50)) black_theme_color = 0xAA0000;
                    if (black_point_in_rect(black_mx, black_my, black_sx+180, black_sy, 50, 50)) black_theme_color = 0x990099;
                }

                if (strcmp(w->black_title, "System Monitor") == 0) {
                    int black_tmax;
                    int black_task_base_y = w->black_y + BLACK_WIN_TITLEBAR + 10 + 20 + 12 + 20 + 30 + 16;
                    black_scheduler_get_tasks(&black_tmax);
                    if (black_my >= black_task_base_y && black_my < w->black_y + w->black_h - 20) {
                        int black_row = (black_my - black_task_base_y) / 12;
                        if (black_row >= 0 && black_row < black_tmax) {
                            black_task_t *black_all = black_scheduler_get_tasks(&black_tmax);
                            int black_valid = 0;
                            for (int j = 0; j < black_tmax; j++) {
                                if (black_all[j].black_state != BLACK_TASK_STATE_UNUSED) {
                                    if (black_valid == black_row) {
                                        black_all[j].black_state = BLACK_TASK_STATE_ZOMBIE;
                                        break;
                                    }
                                    black_valid++;
                                }
                            }
                        }
                    }
                }

                if (strcmp(w->black_title, "File Manager") == 0) {
                    uint64_t black_now = black_pit_get_uptime_ms();
                    int black_is_double = (black_now - black_last_click_time < 500);
                    black_last_click_time = black_now;

                    int black_fx = w->black_x + 10;
                    int black_fy = w->black_y + BLACK_WIN_TITLEBAR + 10;
                    int black_up_x = w->black_x + w->black_w - 70;
                    if (black_point_in_rect(black_mx, black_my, black_up_x, black_fy - 4, 50, 15)) {
                        char *black_last = strrchr(w->black_vfs_path, '/');
                        if (black_last && black_last != w->black_vfs_path) {
                            *black_last = '\0';
                            black_last = strrchr(w->black_vfs_path, '/');
                            if (black_last) *(black_last + 1) = '\0';
                            else strcpy(w->black_vfs_path, "/");
                        } else {
                            strcpy(w->black_vfs_path, "/");
                        }
                        black_gui_needs_redraw = 1;
                        return;
                    }

                    if (black_is_double) {
                        black_fy += 30;
                        black_vfs_node_t *black_dir_node = black_vfs_lookup(w->black_vfs_path);
                        if (black_dir_node && (black_dir_node->black_flags & VFS_DIRECTORY)) {
                            int black_idx = 0;
                            black_dirent_t *black_d;
                            while ((black_d = black_vfs_readdir(black_dir_node, black_idx)) != NULL) {
                                int bx = black_fx + (black_idx % 5) * 80;
                                int by = black_fy + (black_idx / 5) * 70;
                                if (black_point_in_rect(black_mx, black_my, bx, by, 60, 60)) {
                                    if (black_d->black_type & VFS_DIRECTORY) {
                                        strcat(w->black_vfs_path, black_d->black_name);
                                        strcat(w->black_vfs_path, "/");
                                        black_gui_needs_redraw = 1;
                                    } else {
                                        char black_fpath[256];
                                        strcpy(black_fpath, w->black_vfs_path);
                                        strcat(black_fpath, black_d->black_name);
                                        int new_wid = black_gui_create_window("Notepad", black_mx, black_my, 400, 300);
                                        if (new_wid >= 0) {
                                            black_window_t *nw = &black_windows[new_wid];
                                            black_vfs_node_t *black_fnode = black_vfs_lookup(black_fpath);
                                            if (black_fnode) {
                                                uint8_t *black_fbuf = kmalloc(black_fnode->black_length + 1);
                                                if (black_fbuf) {
                                                    black_vfs_read(black_fnode, 0, black_fnode->black_length, black_fbuf);
                                                    black_fbuf[black_fnode->black_length] = '\0';
                                                    black_term_puts(nw, (char*)black_fbuf);
                                                    kfree(black_fbuf);
                                                }
                                            }
                                        }
                                    }
                                    return;
                                }
                                black_idx++;
                            }
                        }
                    }
                }

                if (black_point_in_rect(black_mx, black_my, w->black_x, w->black_y, w->black_w, BLACK_WIN_TITLEBAR)) {
                    black_gui_bring_to_front(wid);
                    if (strcmp(w->black_title, "Notepad") == 0) {
                        int black_save_btn_x = w->black_x + w->black_w - 24 - 20 - 20 - 50;
                        if (black_point_in_rect(black_mx, black_my, black_save_btn_x, w->black_y + 4, 46, 16)) {
                            char black_filename[32];
                            uint32_t black_ticks = (uint32_t)black_pit_get_uptime_ms();
                            char b[10]; int idx=0; while(black_ticks>0) {b[idx++]='0'+(black_ticks%10); black_ticks/=10;} 
                            strcpy(black_filename, "Note_"); int bk=5; while(idx>0) black_filename[bk++]=b[--idx];
                            strcat(black_filename, ".txt");
                            black_vfs_node_t *black_root = black_vfs_get_root();
                            black_vfs_create(black_root, black_filename, VFS_FILE);
                            black_vfs_node_t *black_newf = black_vfs_finddir(black_root, black_filename);
                            if (black_newf) {
                                int black_tlen = 0;
                                for (int y=0; y<=w->black_term_cy; y++) {
                                    for (int x=0; x<BLACK_TERM_COLS; x++) {
                                        char c = w->black_termbuf[y*BLACK_TERM_COLS+x];
                                        if (c) black_tlen++;
                                    }
                                    black_tlen++; // newline
                                }
                                uint8_t *black_wbuf = kmalloc(black_tlen + 1);
                                if (black_wbuf) {
                                    int bix = 0;
                                    for (int y=0; y<=w->black_term_cy; y++) {
                                        for (int x=0; x<BLACK_TERM_COLS; x++) {
                                            char c = w->black_termbuf[y*BLACK_TERM_COLS+x];
                                            if (c) black_wbuf[bix++] = c;
                                        }
                                        black_wbuf[bix++] = '\n';
                                    }
                                    black_vfs_write(black_newf, 0, bix, black_wbuf);
                                    kfree(black_wbuf);
                                    strcpy(w->black_title, black_filename);
                                    black_gui_needs_redraw = 1;
                                }
                            }
                            return;
                        }
                    }
                    if (black_mx >= w->black_x + w->black_w - 24) {
                        black_gui_destroy_window(wid);
                    } else if (black_mx >= w->black_x + w->black_w - 44) {
                        if (w->black_maximized) {
                            w->black_maximized = 0;
                            w->black_x = w->black_saved_x; w->black_y = w->black_saved_y;
                            w->black_w = w->black_saved_w; w->black_h = w->black_saved_h;
                        } else {
                            w->black_maximized = 1;
                            w->black_saved_x = w->black_x; w->black_saved_y = w->black_y;
                            w->black_saved_w = w->black_w; w->black_saved_h = w->black_h;
                            w->black_x = 0; w->black_y = 0;
                            w->black_w = black_fb->black_width; w->black_h = black_fb->black_height - 30;
                        }
                    } else if (black_mx >= w->black_x + w->black_w - 64) {
                        w->black_minimized = 1;
                    } else if (!w->black_maximized) {
                        black_drag_win = wid;
                        black_drag_ox = black_mx - w->black_x;
                        black_drag_oy = black_my - w->black_y;
                    }
                    return;
                }
                
                if (black_point_in_rect(black_mx, black_my, w->black_x, w->black_y, w->black_w, w->black_h)) {
                    black_gui_bring_to_front(wid);
                    return;
                }
            }
        }
    } else if (black_ms->black_buttons & 2 && (black_pressed & 2)) {
        black_menu_state = BLACK_MENU_CONTEXT;
        black_menu_x = black_mx;
        black_menu_y = black_my;
        black_gui_needs_redraw = 1;
    } else {
        black_drag_win = -1;
        black_resize_win = -1;
    }
}

static void black_bga_set_mode(uint16_t black_w, uint16_t black_h, uint16_t black_bpp)
{
    black_outw(0x01CE, 0x04);
    black_outw(0x01CF, 0);
    black_outw(0x01CE, 0x01);
    black_outw(0x01CF, black_w);
    black_outw(0x01CE, 0x02);
    black_outw(0x01CF, black_h);
    black_outw(0x01CE, 0x03);
    black_outw(0x01CF, black_bpp);
    black_outw(0x01CE, 0x04);
    black_outw(0x01CF, 1 | 0x40);
}

void black_gui_init(black_multiboot_info_t *black_mboot)
{
    (void)black_mboot;
    black_fb_init(0xFD000000, 1024, 768, 1024 * 4, 32);
    black_bga_set_mode(1024, 768, 32);
    black_mouse_set_bounds(1024, 768);
    memset(black_windows, 0, sizeof(black_windows));
    black_win_count = 0;
    black_gui_running = 1;
}

void black_gui_update_boot_progress(int black_percent, const char *black_msg)
{
    if (!black_gui_running) return;
    black_framebuffer_t *black_fb = black_fb_get();
    black_fb_clear(0x000000);
    
    int black_cx = black_fb->black_width / 2;
    int black_cy = black_fb->black_height / 2;

    for (int i = 0; i < 4; i++) {
        black_fb_fillrect(black_cx - 60 + (i%2)*65, black_cy - 120 + (i/2)*65, 60, 60, 
            (i==0)?0xFF0000:(i==1)?0x00FF00:(i==2)?0x0000FF:0xFFFF00);
    }
    
    int black_lw = strlen("BLACK OS PHANTOM") * 8;
    black_fb_drawstring(black_cx - (black_lw/2), black_cy + 20, "BLACK OS PHANTOM", 0xFFFFFF, 0x000000);

    int black_bw = 400;
    int black_bh = 20;
    int black_bx = black_cx - (black_bw/2);
    int black_by = black_cy + 60;
    
    black_fb_fillrect(black_bx, black_by, black_bw, black_bh, 0x222222);
    int black_bw_f = (black_bw * black_percent) / 100;
    if (black_bw_f > black_bw) black_bw_f = black_bw;
    for (int i = 0; i < black_bw_f; i += 20) {
        int w = (black_bw_f - i >= 18) ? 18 : (black_bw_f - i);
        black_fb_fillrect(black_bx + i, black_by + 2, w, black_bh - 4, 0x00AA00);
    }

    black_fb_fillrect(black_bx, black_by, black_bw, 2, 0x555555);
    black_fb_fillrect(black_bx, black_by + black_bh - 2, black_bw, 2, 0x555555);
    black_fb_fillrect(black_bx, black_by, 2, black_bh, 0x555555);
    black_fb_fillrect(black_bx + black_bw - 2, black_by, 2, black_bh, 0x555555);

    int black_mw = strlen(black_msg) * 8;
    black_fb_drawstring(black_cx - (black_mw/2), black_by + 30, black_msg, 0xAAAAAA, 0x000000);

    black_fb_swap();
}

void black_gui_loop(void)
{
    if (!black_gui_running) return;
    black_mouse_state_t black_ms;
    black_mouse_get_state(&black_ms);
    black_gui_handle_mouse(&black_ms);
    
    static uint64_t black_last_time = 0;
    uint64_t black_now = black_pit_get_uptime_ms();
    if (black_now - black_last_time > 1000) { black_gui_needs_redraw = 1; black_last_time = black_now; }

    if (!black_gui_needs_redraw) return;
    
    black_fb_clear(BLACK_GUI_COLOR_BG);
    black_gui_draw_matrix_bg();
    
    int black_focused_id = black_gui_get_focused();
    for (int i = 0; i < black_win_count; i++) {
        int wid = black_win_order[i];
        black_gui_draw_window(&black_windows[wid], wid == black_focused_id);
    }
    
    black_gui_draw_taskbar();
    if (black_menu_state != BLACK_MENU_NONE) {
        if (black_menu_state == BLACK_MENU_START) black_gui_draw_menu(black_start_items, black_start_items_count, black_menu_x, black_menu_y);
        else black_gui_draw_menu(black_ctx_items, black_ctx_items_count, black_menu_x, black_menu_y);
    }
    black_gui_draw_cursor(black_ms.black_x, black_ms.black_y);
    black_fb_swap();
    black_gui_needs_redraw = 0;
}
