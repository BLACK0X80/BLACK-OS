#include <kernel/kernel.h>
#include <drivers/framebuffer.h>

#define BLACK_MAX_NOTIFICATIONS 8

typedef struct {
    char black_message[128];
    uint32_t black_timeout;
    int black_active;
} black_notification_t;

static black_notification_t black_notifs[BLACK_MAX_NOTIFICATIONS];

void black_gui_notify_init(void) {
    for (int black_i = 0; black_i < BLACK_MAX_NOTIFICATIONS; black_i++) {
        black_notifs[black_i].black_active = 0;
    }
}

void black_gui_notify_show(const char *black_msg, uint32_t black_duration) {
    for (int black_i = 0; black_i < BLACK_MAX_NOTIFICATIONS; black_i++) {
        if (!black_notifs[black_i].black_active) {
            int black_j = 0;
            while (black_msg[black_j] && black_j < 127) {
                black_notifs[black_i].black_message[black_j] = black_msg[black_j];
                black_j++;
            }
            black_notifs[black_i].black_message[black_j] = '\0';
            black_notifs[black_i].black_timeout = black_duration;
            black_notifs[black_i].black_active = 1;
            return;
        }
    }
}

void black_gui_notify_render(void) {
    int black_y = 10;
    for (int black_i = 0; black_i < BLACK_MAX_NOTIFICATIONS; black_i++) {
        if (black_notifs[black_i].black_active) {
            black_fb_drawstring(700, black_y, black_notifs[black_i].black_message, 0x00FF00, 0x000000);
            black_y += 20;
            if (black_notifs[black_i].black_timeout > 0) {
                black_notifs[black_i].black_timeout--;
            } else {
                black_notifs[black_i].black_active = 0;
            }
        }
    }
}
