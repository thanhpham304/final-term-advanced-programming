#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    char src_path[256];
    char dst_path[256];
    long long buf_size = 0;

    if (scanf("%255s %255s %lld", src_path, dst_path, &buf_size) != 3) {
        printf("-1\n");
        return 0;
    }

    if (buf_size <= 0) {
        printf("-1\n");
        return 0;
    }

    int src_fd = open(src_path, O_RDONLY);
    if (src_fd < 0) {
        printf("-1\n");
        return 0;
    }

    off_t end_pos = lseek(src_fd, 0, SEEK_END);
    if (end_pos < 0) {
        close(src_fd);
        printf("-1\n");
        return 0;
    }

    unsigned char *buf = (unsigned char *)malloc((size_t)buf_size);
    if (buf == NULL) {
        close(src_fd);
        printf("-1\n");
        return 0;
    }

    int dst_fd = open(dst_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dst_fd < 0) {
        free(buf);
        close(src_fd);
        printf("-1\n");
        return 0;
    }

    long long remaining = (long long)end_pos;
    long long total_written = 0;

    while (remaining > 0) {
        size_t chunk = (remaining < buf_size) ? (size_t)remaining : (size_t)buf_size;
        off_t offset = (off_t)(remaining - (long long)chunk);

        if (lseek(src_fd, offset, SEEK_SET) == (off_t)-1) {
            free(buf);
            close(src_fd);
            close(dst_fd);
            printf("-1\n");
            return 0;
        }

        size_t bytes_read = 0;
        while (bytes_read < chunk) {
            ssize_t n = read(src_fd, buf + bytes_read, chunk - bytes_read);
            if (n < 0) {
                if (errno == EINTR) {
                    continue;
                }
                free(buf);
                close(src_fd);
                close(dst_fd);
                printf("-1\n");
                return 0;
            }
            if (n == 0) {
                break;
            }
            bytes_read += (size_t)n;
        }

        if (bytes_read != chunk) {
            free(buf);
            close(src_fd);
            close(dst_fd);
            printf("-1\n");
            return 0;
        }

        for (size_t i = 0, j = bytes_read - 1; i < j; i++, j--) {
            unsigned char tmp = buf[i];
            buf[i] = buf[j];
            buf[j] = tmp;
        }

        size_t bytes_written = 0;
        while (bytes_written < bytes_read) {
            ssize_t w = write(dst_fd, buf + bytes_written, bytes_read - bytes_written);
            if (w < 0) {
                if (errno == EINTR) {
                    continue;
                }
                free(buf);
                close(src_fd);
                close(dst_fd);
                printf("-1\n");
                return 0;
            }
            bytes_written += (size_t)w;
        }

        remaining -= (long long)chunk;
        total_written += (long long)bytes_written;
    }

    free(buf);
    close(src_fd);
    close(dst_fd);
    printf("%lld\n", total_written);
    return 0;
}