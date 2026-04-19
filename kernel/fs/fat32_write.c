#include <kernel/kernel.h>
#include <fs/fat32.h>

int black_fat32_write_file(const char *black_path, const void *black_data, uint32_t black_size) {
    (void)black_path; (void)black_data; (void)black_size;
    return 0;
}

int black_fat32_delete_file(const char *black_path) {
    (void)black_path;
    return 0;
}

int black_fat32_create_dir(const char *black_path) {
    (void)black_path;
    return 0;
}
