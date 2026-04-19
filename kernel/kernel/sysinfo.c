#include <kernel/kernel.h>
#include <drivers/pit.h>

void black_sysinfo_init(void) {
}

const char *black_sysinfo_get_version(void) {
    return "BlackOS PHANTOM v1.0";
}

const char *black_sysinfo_get_arch(void) {
    return "i686";
}

uint32_t black_sysinfo_get_uptime(void) {
    return black_pit_get_uptime_ms() / 1000;
}
