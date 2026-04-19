#include <kernel/kernel.h>
#include <string.h>

#define BLACK_MAX_ENV 64

typedef struct {
    char black_key[64];
    char black_value[128];
} black_env_var_t;

static black_env_var_t black_env[BLACK_MAX_ENV];
static int black_env_count = 0;

void black_shell_env_set(const char *black_key, const char *black_val);

void black_shell_env_init(void) {
    black_env_count = 0;
    black_shell_env_set("PATH", "/bin");
    black_shell_env_set("HOME", "/");
    black_shell_env_set("SHELL", "/bin/shell");
}

void black_shell_env_set(const char *black_key, const char *black_val) {
    for (int black_i = 0; black_i < black_env_count; black_i++) {
        if (strcmp(black_env[black_i].black_key, black_key) == 0) {
            int black_j = 0;
            while (black_val[black_j] && black_j < 127) { black_env[black_i].black_value[black_j] = black_val[black_j]; black_j++; }
            black_env[black_i].black_value[black_j] = '\0';
            return;
        }
    }
    if (black_env_count < BLACK_MAX_ENV) {
        int black_j = 0;
        while (black_key[black_j] && black_j < 63) { black_env[black_env_count].black_key[black_j] = black_key[black_j]; black_j++; }
        black_env[black_env_count].black_key[black_j] = '\0';
        black_j = 0;
        while (black_val[black_j] && black_j < 127) { black_env[black_env_count].black_value[black_j] = black_val[black_j]; black_j++; }
        black_env[black_env_count].black_value[black_j] = '\0';
        black_env_count++;
    }
}

const char *black_shell_env_get(const char *black_key) {
    for (int black_i = 0; black_i < black_env_count; black_i++) {
        if (strcmp(black_env[black_i].black_key, black_key) == 0) {
            return black_env[black_i].black_value;
        }
    }
    return 0;
}
