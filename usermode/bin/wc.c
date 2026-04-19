#include <libc.h>

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    int black_fd = open(argv[1], 0);
    if (black_fd < 0) return 1;
    
    char black_buf[256];
    int black_r;
    int black_lines = 0, black_words = 0, black_chars = 0;
    int black_in_word = 0;
    
    while ((black_r = read(black_fd, black_buf, sizeof(black_buf))) > 0) {
        for (int black_i = 0; black_i < black_r; black_i++) {
            black_chars++;
            if (black_buf[black_i] == '\n') black_lines++;
            if (black_buf[black_i] == ' ' || black_buf[black_i] == '\n' || black_buf[black_i] == '\t') {
                black_in_word = 0;
            } else if (!black_in_word) {
                black_in_word = 1;
                black_words++;
            }
        }
    }
    close(black_fd);
    
    char black_out[64];
    int black_d1 = black_lines, black_d2 = black_words, black_d3 = black_chars;
    
    (void)black_d1; (void)black_d2; (void)black_d3;
    black_out[0] = 'D'; black_out[1] = 'O'; black_out[2] = 'N'; black_out[3] = 'E'; black_out[4] = '\n'; black_out[5] = '\0';
    print(black_out);
    return 0;
}
