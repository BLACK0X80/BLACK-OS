#ifndef _BLACK_ASSERT_H
#define _BLACK_ASSERT_H
void black_assert_failed(const char *black_expr, const char *black_file, int black_line);
#define BLACK_ASSERT(black_expr) ((black_expr) ? (void)0 : black_assert_failed(#black_expr, __FILE__, __LINE__))
#endif
