#include <security/security.h>
#include <kernel/kernel.h>
#include <drivers/serial.h>
#include <drivers/pit.h>
#include <drivers/pci.h>
#include <mm/pmm.h>
#include <string.h>

static uint32_t black_cpu_features = 0;
static black_user_t black_users[MAX_USERS];
static black_group_t black_groups[MAX_GROUPS];
static uint32_t black_aslr_seed = 0;

static black_aslr_config_t black_aslr_cfg = { 1, 8, 8, 5 };

uint32_t __stack_chk_guard = STACK_CHK_GUARD;

static uint32_t black_fnv1a(uint32_t black_hash, uint32_t black_val)
{
    uint8_t *black_b = (uint8_t *)&black_val;
    for (int black_i = 0; black_i < 4; black_i++) {
        black_hash ^= black_b[black_i];
        black_hash *= 0x01000193;
    }
    return black_hash;
}

static void black_sha256_simple(const char *black_input, char *black_output)
{
    uint32_t black_hash = 0x811C9DC5;
    while (*black_input) {
        black_hash ^= (uint8_t)*black_input++;
        black_hash *= 0x01000193;
    }
    uint32_t black_h2 = black_hash;
    black_h2 ^= black_h2 >> 16;
    black_h2 *= 0x45d9f3b;
    black_h2 ^= black_h2 >> 16;

    uint32_t black_vals[4] = { black_hash, black_h2, black_hash ^ 0xDEADBEEF, black_h2 ^ 0xCAFEBABE };
    int black_pos = 0;
    for (int black_v = 0; black_v < 4; black_v++) {
        for (int black_i = 7; black_i >= 0; black_i--) {
            int black_n = (black_vals[black_v] >> (black_i * 4)) & 0xF;
            black_output[black_pos++] = black_n < 10 ? '0' + black_n : 'a' + black_n - 10;
        }
    }
    black_output[32] = '\0';
}

void black_security_init_stack_canary(void)
{
    uint32_t black_entropy = 0x811C9DC5;
    uint32_t black_lo, black_hi;
    __asm__ volatile("rdtsc" : "=a"(black_lo), "=d"(black_hi));
    black_entropy = black_fnv1a(black_entropy, black_lo);
    black_entropy = black_fnv1a(black_entropy, black_hi);
    black_entropy = black_fnv1a(black_entropy, (uint32_t)black_pit_get_ticks());
    black_entropy = black_fnv1a(black_entropy, black_pmm_get_free_memory());
    uint32_t black_esp;
    __asm__ volatile("mov %%esp, %0" : "=r"(black_esp));
    black_entropy = black_fnv1a(black_entropy, black_esp);
    __asm__ volatile("rdtsc" : "=a"(black_lo), "=d"(black_hi));
    black_entropy = black_fnv1a(black_entropy, black_lo ^ black_hi);
    black_entropy ^= (black_entropy >> 16);
    black_entropy *= 0x45d9f3b;
    black_entropy ^= (black_entropy >> 16);
    if (black_entropy == 0) black_entropy = 0xB1AC4B05;
    __stack_chk_guard = black_entropy;
}

void __attribute__((noreturn)) __stack_chk_fail(void)
{
    __asm__ volatile("cli");
    black_serial_puts("\n[SECURITY] *** STACK SMASHING DETECTED ***\n");
    __asm__ volatile("hlt");
    while(1);
}

int black_user_add(const char *black_name, const char *black_pass, uint32_t black_uid, uint32_t black_gid)
{
    for (int black_i = 0; black_i < MAX_USERS; black_i++) {
        if (black_users[black_i].black_active && black_users[black_i].black_uid == black_uid)
            return -1;
    }
    for (int black_i = 0; black_i < MAX_USERS; black_i++) {
        if (!black_users[black_i].black_active) {
            black_users[black_i].black_uid = black_uid;
            black_users[black_i].black_gid = black_gid;
            strncpy(black_users[black_i].black_username, black_name, MAX_USERNAME_LEN - 1);
            black_sha256_simple(black_pass, black_users[black_i].black_password_hash);
            strcpy(black_users[black_i].black_home_dir, "/home/");
            strcat(black_users[black_i].black_home_dir, black_name);
            strcpy(black_users[black_i].black_shell, "/bin/sh");
            black_users[black_i].black_active = 1;
            return 0;
        }
    }
    return -2;
}

int black_user_authenticate(const char *black_name, const char *black_pass)
{
    for (int black_i = 0; black_i < MAX_USERS; black_i++) {
        if (black_users[black_i].black_active &&
            strcmp(black_users[black_i].black_username, black_name) == 0) {
            char black_hash[65];
            black_sha256_simple(black_pass, black_hash);
            if (strcmp(black_users[black_i].black_password_hash, black_hash) == 0)
                return (int)black_users[black_i].black_uid;
            return -1;
        }
    }
    return -1;
}

black_user_t *black_user_get_by_uid(uint32_t black_uid)
{
    for (int black_i = 0; black_i < MAX_USERS; black_i++) {
        if (black_users[black_i].black_active && black_users[black_i].black_uid == black_uid)
            return &black_users[black_i];
    }
    return NULL;
}

black_user_t *black_user_get_by_name(const char *black_name)
{
    for (int black_i = 0; black_i < MAX_USERS; black_i++) {
        if (black_users[black_i].black_active &&
            strcmp(black_users[black_i].black_username, black_name) == 0)
            return &black_users[black_i];
    }
    return NULL;
}

int black_group_add(const char *black_name, uint32_t black_gid)
{
    for (int black_i = 0; black_i < MAX_GROUPS; black_i++) {
        if (!black_groups[black_i].black_active) {
            black_groups[black_i].black_gid = black_gid;
            strncpy(black_groups[black_i].black_groupname, black_name, MAX_GROUPNAME_LEN - 1);
            black_groups[black_i].black_member_count = 0;
            black_groups[black_i].black_active = 1;
            return 0;
        }
    }
    return -2;
}

int black_group_add_member(uint32_t black_gid, uint32_t black_uid)
{
    for (int black_i = 0; black_i < MAX_GROUPS; black_i++) {
        if (black_groups[black_i].black_active && black_groups[black_i].black_gid == black_gid) {
            if (black_groups[black_i].black_member_count >= MAX_USERS) return -2;
            black_groups[black_i].black_members[black_groups[black_i].black_member_count++] = black_uid;
            return 0;
        }
    }
    return -1;
}

int black_group_is_member(uint32_t black_gid, uint32_t black_uid)
{
    for (int black_i = 0; black_i < MAX_GROUPS; black_i++) {
        if (black_groups[black_i].black_active && black_groups[black_i].black_gid == black_gid) {
            for (int black_j = 0; black_j < black_groups[black_i].black_member_count; black_j++) {
                if (black_groups[black_i].black_members[black_j] == black_uid) return 1;
            }
        }
    }
    return 0;
}

void black_aslr_init(void)
{
    uint32_t black_lo, black_hi;
    __asm__ volatile("rdtsc" : "=a"(black_lo), "=d"(black_hi));
    black_aslr_seed = black_lo ^ black_hi;
}

static uint32_t black_aslr_random(void)
{
    black_aslr_seed = black_aslr_seed * 1103515245 + 12345;
    return black_aslr_seed;
}

uint32_t black_aslr_randomize_stack(uint32_t black_base)
{
    if (!black_aslr_cfg.black_enabled) return black_base;
    return black_base - ((black_aslr_random() & ((1 << black_aslr_cfg.black_stack_entropy_bits) - 1)) << 12);
}

uint32_t black_aslr_randomize_mmap(uint32_t black_base)
{
    if (!black_aslr_cfg.black_enabled) return black_base;
    return black_base + ((black_aslr_random() & ((1 << black_aslr_cfg.black_mmap_entropy_bits) - 1)) << 12);
}

uint32_t black_aslr_randomize_heap(uint32_t black_base)
{
    if (!black_aslr_cfg.black_enabled) return black_base;
    return black_base + ((black_aslr_random() & ((1 << black_aslr_cfg.black_heap_entropy_bits) - 1)) << 12);
}

uint32_t black_security_get_cpu_features(void) { return black_cpu_features; }
int black_security_has_nx(void) { return (black_cpu_features & CPU_FEATURE_NX) != 0; }
int black_security_has_smep(void) { return (black_cpu_features & CPU_FEATURE_SMEP) != 0; }
int black_security_has_smap(void) { return (black_cpu_features & CPU_FEATURE_SMAP) != 0; }

void black_security_enable_smep(void)
{
    if (!(black_cpu_features & CPU_FEATURE_SMEP)) return;
    uint32_t black_cr4;
    __asm__ volatile("mov %%cr4, %0" : "=r"(black_cr4));
    black_cr4 |= (1 << 20);
    __asm__ volatile("mov %0, %%cr4" : : "r"(black_cr4));
}

void black_security_enable_smap(void)
{
    if (!(black_cpu_features & CPU_FEATURE_SMAP)) return;
    uint32_t black_cr4;
    __asm__ volatile("mov %%cr4, %0" : "=r"(black_cr4));
    black_cr4 |= (1 << 21);
    __asm__ volatile("mov %0, %%cr4" : : "r"(black_cr4));
}

void black_security_init(void)
{
    memset(black_users, 0, sizeof(black_users));
    memset(black_groups, 0, sizeof(black_groups));

    uint32_t black_eax, black_ebx, black_ecx, black_edx;
    black_cpuid(1, &black_eax, &black_ebx, &black_ecx, &black_edx);
    if (black_edx & (1 << 6)) black_cpu_features |= CPU_FEATURE_PAE;

    black_cpuid(0x80000001, &black_eax, &black_ebx, &black_ecx, &black_edx);
    if (black_edx & (1 << 20)) black_cpu_features |= CPU_FEATURE_NX;

    black_cpuid(7, &black_eax, &black_ebx, &black_ecx, &black_edx);

    black_security_init_stack_canary();
    black_aslr_init();

    black_group_add("root", ROOT_GID);
    black_group_add("wheel", 10);
    black_group_add("users", 100);
    black_user_add("root", "black", ROOT_UID, ROOT_GID);
    black_group_add_member(ROOT_GID, ROOT_UID);
}
