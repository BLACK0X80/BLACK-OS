#ifndef _BLACK_SYSINFO_H
#define _BLACK_SYSINFO_H
#include <stdint.h>
void black_sysinfo_init(void);
const char *black_sysinfo_get_version(void);
const char *black_sysinfo_get_arch(void);
uint32_t black_sysinfo_get_uptime(void);
#endif
