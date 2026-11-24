#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#define BUF_SIZE 4096

int main(void) {
    char file_path[256];

    if (scanf("%255s", file_path) != 1) {
        printf("-1\n");
        return 0;
    }

    int fd = open(file_path, O_RDONLY);
    if (fd < 0) {
        printf("-1\n");
        return 0;
    }

    unsigned char buf[BUF_SIZE];
    long long total_bytes = 0;
    long long newline_count = 0;
    long long digit_count = 0;

    while (1) {
        ssize_t n = read(fd, buf, sizeof(buf));
        if (n < 0) {
            if (errno == EINTR) {
                continue; 
            }
            close(fd);
            printf("-1\n");
            return 0;
        }
        if (n == 0) {
            break; 
        }

        total_bytes += n;
        for (ssize_t i = 0; i < n; i++) {
            if (buf[i] == '\n') {
                newline_count++;
            } else if (buf[i] >= '0' && buf[i] <= '9') {
                digit_count++;
            }
        }
    }

    close(fd);
    printf("%lld %lld %lld\n", total_bytes, newline_count, digit_count);
    return 0;
}