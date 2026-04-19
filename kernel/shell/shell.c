#include <kernel/shell.h>
#include <kernel/kernel.h>
#include <drivers/vga.h>
#include <drivers/serial.h>
#include <drivers/keyboard.h>
#include <drivers/pit.h>
#include <mm/pmm.h>
#include <mm/heap.h>
#include <kernel/scheduler.h>
#include <string.h>

#define BLACK_CMD_BUF_SIZE  256

static char black_cmd_buf[BLACK_CMD_BUF_SIZE];
static int  black_cmd_pos = 0;
char black_cwd[256] = "/";

static volatile int black_shell_ready = 0;

static void black_shell_prompt(void)
{
    black_vga_set_color(0x02, 0x00);
    black_vga_puts("black");
    black_vga_set_color(0x0A, 0x00);
    black_vga_puts("@");
    black_vga_set_color(0x02, 0x00);
    black_vga_puts("phantom");
    black_vga_set_color(0x0A, 0x00);
    black_vga_puts(":");
    black_vga_set_color(0x0A, 0x00);
    black_vga_puts(black_cwd);
    black_vga_set_color(0x0A, 0x00);
    black_vga_puts("$ ");
}

static void black_cmd_help(void)
{
    black_vga_set_color(0x0A, 0x00);
    black_vga_puts("\n  BlackOS PHANTOM Shell Commands\n");
    black_vga_puts("  ==============================\n\n");
    black_vga_set_color(0x02, 0x00);
    black_vga_puts("  help      - Summary of commands\n");
    black_vga_puts("  clear     - Clean the view\n");
    black_vga_puts("  echo      - Output literal text\n");
    black_vga_puts("  date      - Core system time\n");
    black_vga_puts("  calc      - Mathematical evaluator\n");
    black_vga_puts("  ls        - List system objects\n");
    black_vga_puts("  mkdir     - Allocate subsystem directory\n");
    black_vga_puts("  uname     - Architecture identity\n");
    black_vga_puts("  meminfo   - Volatile memory stats\n");
    black_vga_puts("  tasks     - Execution registry (ps)\n");
    black_vga_puts("  uptime    - Continuous operation timer\n");
    black_vga_puts("  whoami    - Access clearance level\n");
    black_vga_puts("  matrix    - Digital downpour\n");
    black_vga_puts("  version   - Kernel release string\n");
    black_vga_puts("  halt      - Terminate kernel\n");
    black_vga_putchar('\n');
}

static void black_cmd_uname(void)
{
    black_vga_set_color(0x0A, 0x00);
    black_vga_puts("\n  BlackOS PHANTOM v");
    black_vga_puts(BLACK_VERSION_STRING);
    black_vga_puts(" i686 ");
    black_vga_puts(BLACK_CODENAME);
    black_vga_puts("\n\n");
}

static void black_cmd_meminfo(void)
{
    black_vga_set_color(0x0A, 0x00);
    black_vga_puts("\n  === Memory Report ===\n");
    black_vga_set_color(0x02, 0x00);
    black_vga_puts("  Total:  ");
    black_vga_put_dec(black_pmm_get_total_memory() / (1024 * 1024));
    black_vga_puts(" MB\n  Free:   ");
    black_vga_put_dec(black_pmm_get_free_memory() / (1024 * 1024));
    black_vga_puts(" MB\n  Used:   ");
    black_vga_put_dec(black_pmm_get_used_memory() / (1024 * 1024));
    black_vga_puts(" MB\n  Heap:   ");
    black_vga_put_dec(black_heap_get_used() / 1024);
    black_vga_puts(" KB used / ");
    black_vga_put_dec(black_heap_get_free() / 1024);
    black_vga_puts(" KB free\n\n");
}

static void black_cmd_tasks(void)
{
    int black_max;
    black_task_t *black_all = black_scheduler_get_tasks(&black_max);
    static const char *black_state_names[] = { "UNUSED", "RUN", "READY", "BLOCK", "SLEEP", "ZOMBIE" };

    black_vga_set_color(0x0A, 0x00);
    black_vga_puts("\n  TID  NAME             STATE   CPU TIME\n");
    black_vga_puts("  ---  ----             -----   --------\n");
    black_vga_set_color(0x02, 0x00);

    for (int black_i = 0; black_i < black_max; black_i++) {
        if (black_all[black_i].black_state == BLACK_TASK_STATE_UNUSED) continue;
        black_vga_puts("  ");
        black_vga_put_dec(black_all[black_i].black_tid);
        black_vga_puts("    ");
        black_vga_puts(black_all[black_i].black_name);
        int black_pad = 17 - (int)strlen(black_all[black_i].black_name);
        while (black_pad-- > 0) black_vga_putchar(' ');
        if (black_all[black_i].black_state < 6)
            black_vga_puts(black_state_names[black_all[black_i].black_state]);
        black_vga_puts("    ");
        black_vga_put_dec((uint32_t)black_all[black_i].black_cpu_time);
        black_vga_putchar('\n');
    }
    black_vga_putchar('\n');
}

static void black_cmd_matrix(void)
{
    black_vga_clear();
    uint16_t *black_buf = (uint16_t *)0xC00B8000;
    uint32_t black_seed = (uint32_t)black_pit_get_ticks();

    for (int black_frame = 0; black_frame < 300; black_frame++) {
        for (int black_col = 0; black_col < 80; black_col++) {
            black_seed = black_seed * 1103515245 + 12345;
            if ((black_seed >> 16) % 3 == 0) {
                for (int black_row = 24; black_row > 0; black_row--) {
                    black_buf[black_row * 80 + black_col] = black_buf[(black_row - 1) * 80 + black_col];
                }
                black_seed = black_seed * 1103515245 + 12345;
                char black_c = '0' + ((black_seed >> 16) % 10);
                uint8_t black_bright = ((black_seed >> 24) % 3 == 0) ? 0x0A : 0x02;
                black_buf[black_col] = (uint16_t)(black_c | (black_bright << 8));
            }
        }
        for (volatile int black_d = 0; black_d < 200000; black_d++);
    }
    black_vga_clear();
}

static void black_cmd_version(void)
{
    black_vga_set_color(0x0A, 0x00);
    black_vga_puts("\n  BlackOS v");
    black_vga_puts(BLACK_VERSION_STRING);
    black_vga_puts(" \"");
    black_vga_puts(BLACK_CODENAME);
    black_vga_puts("\"\n");
    black_vga_set_color(0x02, 0x00);
    black_vga_puts("  MLFQ Scheduler | COW VMM | SHA-256 Crypto\n");
    black_vga_puts("  Rust-powered security | Ticket spinlocks\n\n");
}

static void black_cmd_date(void)
{
    char black_time[32];
    uint64_t black_ms = black_pit_get_uptime_ms();
    uint32_t black_s = (uint32_t)(black_ms / 1000);
    uint32_t black_m = black_s / 60; black_s %= 60;
    uint32_t black_hr = black_m / 60; black_m %= 60;
    black_vga_puts("\n  System Time: ");
    black_time[0] = '0' + (black_hr / 10); black_time[1] = '0' + (black_hr % 10);
    black_time[2] = ':';
    black_time[3] = '0' + (black_m / 10); black_time[4] = '0' + (black_m % 10);
    black_time[5] = ':';
    black_time[6] = '0' + (black_s / 10); black_time[7] = '0' + (black_s % 10);
    black_time[8] = 0;
    black_vga_puts(black_time);
    black_vga_puts(" (UPTIME UTC)\n\n");
}

static void black_cmd_echo(const char *black_cmd)
{
    black_vga_puts("\n  ");
    const char *black_args = black_cmd + 4;
    while (*black_args == ' ') black_args++;
    black_vga_puts(black_args);
    black_vga_puts("\n\n");
}

static void black_cmd_calc(const char *black_cmd)
{
    const char *p = black_cmd + 4;
    while (*p == ' ') p++;
    int black_a = 0, black_b = 0;
    char black_op = 0;
    while (*p >= '0' && *p <= '9') { black_a = black_a * 10 + (*p - '0'); p++; }
    while (*p == ' ') p++;
    if (*p == '+' || *p == '-' || *p == '*' || *p == '/') { black_op = *p; p++; }
    while (*p == ' ') p++;
    while (*p >= '0' && *p <= '9') { black_b = black_b * 10 + (*p - '0'); p++; }
    
    black_vga_puts("\n  Result: ");
    if (black_op == '+') black_vga_put_dec(black_a + black_b);
    else if (black_op == '-') { if (black_a < black_b) { black_vga_putchar('-'); black_vga_put_dec(black_b - black_a); } else { black_vga_put_dec(black_a - black_b); } }
    else if (black_op == '*') black_vga_put_dec(black_a * black_b);
    else if (black_op == '/') { if (black_b == 0) black_vga_puts("Err: Div0"); else black_vga_put_dec(black_a / black_b); }
    else black_vga_puts("Err: Parse");
    black_vga_puts("\n\n");
}

static void black_cmd_mkdir(const char *black_cmd)
{
    const char *p = black_cmd + 5;
    while (*p == ' ') p++;
    if (*p == 0) { black_vga_puts("\n  mkdir: missing operand\n\n"); return; }
    black_vga_puts("\n  [VFS] Allocated sub-tree node: ");
    black_vga_puts(p);
    black_vga_puts("\n\n");
}

static void black_cmd_ls(void)
{
    black_vga_puts("\n  [ . ]           [ .. ]          [ sys ]\n");
    black_vga_puts("  [ dev ]         [ tmp ]         kernel.bin\n");
    black_vga_puts("  config.json     readme.txt      secrets.dat\n\n");
}

void black_execute_cmd(const char *black_cmd)
{
    if (strlen(black_cmd) == 0) return;

    if (strcmp(black_cmd, "help") == 0) black_cmd_help();
    else if (strcmp(black_cmd, "clear") == 0) black_vga_clear();
    else if (strcmp(black_cmd, "uname") == 0 || strcmp(black_cmd, "uname -a") == 0) black_cmd_uname();
    else if (strcmp(black_cmd, "meminfo") == 0 || strcmp(black_cmd, "free") == 0) black_cmd_meminfo();
    else if (strcmp(black_cmd, "tasks") == 0 || strcmp(black_cmd, "ps") == 0) black_cmd_tasks();
    else if (strcmp(black_cmd, "ls") == 0) black_cmd_ls();
    else if (strcmp(black_cmd, "date") == 0 || strcmp(black_cmd, "time") == 0) black_cmd_date();
    else if (strncmp(black_cmd, "echo ", 5) == 0) black_cmd_echo(black_cmd);
    else if (strncmp(black_cmd, "calc ", 5) == 0) black_cmd_calc(black_cmd);
    else if (strncmp(black_cmd, "mkdir ", 6) == 0) black_cmd_mkdir(black_cmd);
    else if (strcmp(black_cmd, "uptime") == 0) {
        uint64_t black_ms = black_pit_get_uptime_ms();
        black_vga_puts("\n  Continuous uptime: ");
        black_vga_put_dec((uint32_t)(black_ms / 1000));
        black_vga_puts(" seconds\n\n");
    }
    else if (strcmp(black_cmd, "whoami") == 0) { black_vga_puts("\n  phantom_root\n\n"); }
    else if (strcmp(black_cmd, "matrix") == 0) black_cmd_matrix();
    else if (strcmp(black_cmd, "version") == 0) black_cmd_version();
    else if (strcmp(black_cmd, "halt") == 0 || strcmp(black_cmd, "shutdown") == 0) {
        black_vga_puts("\n  Terminating subsystems...\n");
        black_cli();
        for (;;) black_hlt();
    }
    else {
        black_vga_set_color(0x04, 0x00);
        black_vga_puts("\n  [-] UNRECOGNIZED INSTRUCTION: ");
        black_vga_puts(black_cmd);
        black_vga_putchar('\n');
        black_vga_set_color(0x02, 0x00);
        black_vga_putchar('\n');
    }
}

void black_shell_input(char black_c)
{
    if (!black_shell_ready) return;

    if (black_c == '\n') {
        black_vga_putchar('\n');
        black_cmd_buf[black_cmd_pos] = '\0';
        black_execute_cmd(black_cmd_buf);
        black_cmd_pos = 0;
        black_shell_prompt();
    } else if (black_c == '\b') {
        if (black_cmd_pos > 0) {
            black_cmd_pos--;
            black_vga_putchar('\b');
        }
    } else if (black_cmd_pos < BLACK_CMD_BUF_SIZE - 1) {
        black_cmd_buf[black_cmd_pos++] = black_c;
        black_vga_putchar(black_c);
    }
}

void black_shell_init(void)
{
    black_cmd_pos = 0;
    black_shell_ready = 1;
    extern void black_gui_key_input(char);
    black_keyboard_set_callback(black_gui_key_input);
}

void black_shell_run(void)
{
    black_shell_prompt();
    while (1) {
        extern void black_gui_loop(void);
        black_gui_loop();
        black_hlt();
    }
}

void black_shell_reset_cwd(void)
{
    strcpy(black_cwd, "/");
}
