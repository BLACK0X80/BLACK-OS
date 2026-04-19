#include <drivers/keyboard.h>
#include <arch/x86/idt.h>
#include <kernel/kernel.h>

static void (*black_kb_callback)(char) = NULL;

#define BLACK_KB_BUF_SIZE   256
static black_key_event_t black_kb_event_buf[BLACK_KB_BUF_SIZE];
static volatile int black_kb_head = 0;
static volatile int black_kb_tail = 0;

static int black_kb_shift = 0;
static int black_kb_ctrl = 0;
static int black_kb_alt = 0;
static int black_kb_extended = 0;

static const char black_scancode_to_ascii[128] = {
    0,27,'1','2','3','4','5','6','7','8','9','0','-','=','\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,'\\','z','x','c','v','b','n','m',',','.','/',0,
    '*',0,' ',0,0,0,0,0,0,0,0,0,0,0,0,0,
    '7','8','9','-','4','5','6','+','1','2','3','0','.',0,0,0,0,0
};

static const char black_scancode_to_ascii_shift[128] = {
    0,27,'!','@','#','$','%','^','&','*','(',')','_','+','\b',
    '\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n',
    0,'A','S','D','F','G','H','J','K','L',':','"','~',
    0,'|','Z','X','C','V','B','N','M','<','>','?',0,
    '*',0,' ',0,0,0,0,0,0,0,0,0,0,0,0,0,
    '7','8','9','-','4','5','6','+','1','2','3','0','.',0,0,0,0,0
};

static void black_kb_push_event(black_key_event_t *black_ev)
{
    int black_next = (black_kb_head + 1) % BLACK_KB_BUF_SIZE;
    if (black_next == black_kb_tail) return;
    black_kb_event_buf[black_kb_head] = *black_ev;
    black_kb_head = black_next;
}

static void black_kb_irq_handler(black_registers_t *black_regs)
{
    (void)black_regs;
    uint8_t black_sc = black_inb(0x60);

    if (black_sc == 0xE0) {
        black_kb_extended = 1;
        return;
    }

    int black_released = black_sc & 0x80;
    uint8_t black_code = black_sc & 0x7F;

    if (black_code == 0x2A || black_code == 0x36) { black_kb_shift = !black_released; black_kb_extended = 0; return; }
    if (black_code == 0x1D) { black_kb_ctrl = !black_released; black_kb_extended = 0; return; }
    if (black_code == 0x38) { black_kb_alt = !black_released; black_kb_extended = 0; return; }

    char black_ascii = 0;
    if (black_code < 128) {
        black_ascii = black_kb_shift ?
            black_scancode_to_ascii_shift[black_code] :
            black_scancode_to_ascii[black_code];
    }

    black_key_event_t black_ev;
    black_ev.black_scancode = black_code;
    black_ev.black_ascii = black_ascii;
    black_ev.black_modifiers = 0;
    if (black_kb_shift) black_ev.black_modifiers |= KEY_MOD_SHIFT;
    if (black_kb_ctrl) black_ev.black_modifiers |= KEY_MOD_CTRL;
    if (black_kb_alt) black_ev.black_modifiers |= KEY_MOD_ALT;
    black_ev.black_pressed = !black_released;
    black_ev.black_extended = black_kb_extended;

    black_kb_push_event(&black_ev);

    if (!black_released && black_ascii && black_kb_callback) {
        black_kb_callback(black_ascii);
    }

    black_kb_extended = 0;
}

void black_keyboard_init(void)
{
    black_idt_set_handler(33, black_kb_irq_handler);
}

void black_keyboard_set_callback(void (*black_cb)(char))
{
    black_kb_callback = black_cb;
}

int black_keyboard_get_event(black_key_event_t *black_ev)
{
    if (black_kb_tail == black_kb_head) return 0;
    *black_ev = black_kb_event_buf[black_kb_tail];
    black_kb_tail = (black_kb_tail + 1) % BLACK_KB_BUF_SIZE;
    return 1;
}

char black_keyboard_getchar(void)
{
    black_key_event_t black_ev;
    while (1) {
        if (black_keyboard_get_event(&black_ev)) {
            if (black_ev.black_pressed && black_ev.black_ascii)
                return black_ev.black_ascii;
        }
        black_hlt();
    }
}
