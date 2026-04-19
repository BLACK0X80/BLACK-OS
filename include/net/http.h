#ifndef _BLACK_HTTP_H
#define _BLACK_HTTP_H
#include <stdint.h>
int black_http_get(const char *black_host, const char *black_path, void *black_buf, uint32_t black_max);
#endif
