#ifndef _BLACK_STDDEF_H
#define _BLACK_STDDEF_H

#include <stdint.h>

#define offsetof(black_type, black_member) __builtin_offsetof(black_type, black_member)

typedef int black_wchar_t;

#endif
