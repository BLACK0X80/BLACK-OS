#ifndef _BLACK_SHELL_FEATURES_H
#define _BLACK_SHELL_FEATURES_H
void black_shell_env_init(void);
void black_shell_env_set(const char *black_key, const char *black_val);
const char *black_shell_env_get(const char *black_key);
void black_shell_jobs_init(void);
int black_shell_jobs_add(uint32_t black_pid, const char *black_name);
void black_shell_jobs_list(void);
void black_shell_script_init(void);
int black_shell_script_run(const char *black_path);
#endif
