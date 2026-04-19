#ifndef _BLACK_SHELL_H
#define _BLACK_SHELL_H

void black_shell_init(void);
void black_shell_run(void);
void black_shell_input(char black_c);
void black_shell_reset_cwd(void);

#define shell_init      black_shell_init
#define shell_run       black_shell_run
#define shell_input     black_shell_input
#define shell_reset_cwd black_shell_reset_cwd

#endif
