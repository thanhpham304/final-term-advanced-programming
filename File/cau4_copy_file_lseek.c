#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#define BUF_SIZE 4096

int main(void) {
    char src_path[256];
    char dst_path[256];
    long long offset = 0;
    long long max_bytes = 0;

    if (scanf("%255s %255s %lld %lld", src_path, dst_path, &offset, &max_bytes) != 4) {
        printf("-1\n");
        return 0;
    }

    if (offset < 0 || max_bytes < 0) {
        printf("-1\n");
        return 0;
    }

    int src_fd = open(src_path, O_RDONLY);
    if (src_fd < 0) {
        printf("-1\n");
        return 0;
    }

    if (lseek(src_fd, (off_t)offset, SEEK_SET) == (off_t)-1) {
        close(src_fd);
        printf("-1\n");
        return 0;
    }

    int dst_fd = open(dst_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dst_fd < 0) {
        close(src_fd);
        printf("-1\n");
        return 0;
    }

    unsigned char buf[BUF_SIZE];
    long long total_copied = 0;

    while (total_copied < max_bytes) {
        long long remaining = max_bytes - total_copied;
        size_t to_read = (remaining < (long long)sizeof(buf)) ? (size_t)remaining : sizeof(buf);

        ssize_t n = read(src_fd, buf, to_read);
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
            break; 
        }

        ssize_t written = 0;
        while (written < n) {
            ssize_t w = write(dst_fd, buf + written, (size_t)(n - written));
            if (w < 0) {
                if (errno == EINTR) {
                    continue;
                }
                close(src_fd);
                close(dst_fd);
                printf("-1\n");
                return 0;
            }
            written += w;
        }

        total_copied += n;
    }

    close(src_fd);
    close(dst_fd);
    printf("%lld\n", total_copied);
    return 0;
}