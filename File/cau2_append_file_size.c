#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUF_SIZE 4096

int main(void) {
    char src_path[256];
    char dst_path[256];

    if (scanf("%255s %255s", src_path, dst_path) != 2) {
        printf("-1\n");
        return 0;
    }

    int src_fd = open(src_path, O_RDONLY);
    if (src_fd < 0) {
        printf("-1\n");
        return 0;
    }

    int dst_fd = open(dst_path, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (dst_fd < 0) {
        close(src_fd);
        printf("-1\n");
        return 0;
    }

    unsigned char buf[BUF_SIZE];
    while (1) {
        ssize_t n = read(src_fd, buf, sizeof(buf));
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            close(src_fd);
            close(dst_fd);
            printf("-1\n");
            return 0;
        }
        if (n == 0) {
            break; /* Đọc hết tệp nguồn */
        }

        ssize_t total_written = 0;
        while (total_written < n) {
            ssize_t w = write(dst_fd, buf + total_written, (size_t)(n - total_written));
            if (w < 0) {
                if (errno == EINTR) {
                    continue;
                }
                close(src_fd);
                close(dst_fd);
                printf("-1\n");
                return 0;
            }
            total_written += w;
        }
    }

    struct stat st;
    if (fstat(dst_fd, &st) < 0) {
        close(src_fd);
        close(dst_fd);
        printf("-1\n");
        return 0;
    }

    long long final_size = (long long)st.st_size;

    close(src_fd);
    close(dst_fd);

    printf("%lld\n", final_size);
    return 0;
}