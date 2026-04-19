#ifndef _BLACK_ROUTE_H
#define _BLACK_ROUTE_H
#include <stdint.h>
void black_route_init(void);
int black_route_add(uint32_t black_dst, uint32_t black_mask, uint32_t black_gw, int black_iface);
uint32_t black_route_lookup(uint32_t black_dst);
#endif
