#ifndef _BLACK_SECURITY_H
#define _BLACK_SECURITY_H

#include <stdint.h>

#define CPU_FEATURE_PAE   0x01
#define CPU_FEATURE_NX    0x02
#define CPU_FEATURE_SMEP  0x04
#define CPU_FEATURE_SMAP  0x08

#define MAX_USERS   32
#define MAX_GROUPS  16
#define ROOT_UID    0
#define ROOT_GID    0
#define MAX_USERNAME_LEN  32
#define MAX_GROUPNAME_LEN 32
#define STACK_CHK_GUARD   0xDEADC0DE

typedef struct {
    uint32_t black_uid;
    uint32_t black_gid;
    char     black_username[MAX_USERNAME_LEN];
    char     black_password_hash[65];
    char     black_home_dir[128];
    char     black_shell[64];
    int      black_active;
} black_user_t;

typedef black_user_t user_t;

typedef struct {
    uint32_t black_gid;
    char     black_groupname[MAX_GROUPNAME_LEN];
    uint32_t black_members[MAX_USERS];
    int      black_member_count;
    int      black_active;
} black_group_t;

typedef black_group_t group_t;

typedef struct {
    int      black_enabled;
    uint32_t black_stack_entropy_bits;
    uint32_t black_mmap_entropy_bits;
    uint32_t black_heap_entropy_bits;
} black_aslr_config_t;

void black_security_init(void);
void black_security_init_stack_canary(void);
uint32_t black_security_get_cpu_features(void);
int  black_security_has_nx(void);
int  black_security_has_smep(void);
int  black_security_has_smap(void);
void black_security_enable_smep(void);
void black_security_enable_smap(void);

int  black_user_add(const char *black_name, const char *black_pass, uint32_t black_uid, uint32_t black_gid);
int  black_user_authenticate(const char *black_name, const char *black_pass);
black_user_t *black_user_get_by_uid(uint32_t black_uid);
black_user_t *black_user_get_by_name(const char *black_name);

int  black_group_add(const char *black_name, uint32_t black_gid);
int  black_group_add_member(uint32_t black_gid, uint32_t black_uid);
int  black_group_is_member(uint32_t black_gid, uint32_t black_uid);

void black_aslr_init(void);
uint32_t black_aslr_randomize_stack(uint32_t black_base);
uint32_t black_aslr_randomize_mmap(uint32_t black_base);
uint32_t black_aslr_randomize_heap(uint32_t black_base);

extern uint32_t __stack_chk_guard;

#define security_init              black_security_init
#define security_init_stack_canary black_security_init_stack_canary
#define security_get_cpu_features  black_security_get_cpu_features
#define user_add                   black_user_add
#define user_authenticate          black_user_authenticate
#define user_get_by_uid            black_user_get_by_uid
#define group_add                  black_group_add
#define group_add_member           black_group_add_member
#define aslr_init                  black_aslr_init

#endif
