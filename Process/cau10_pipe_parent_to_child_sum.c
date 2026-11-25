#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static int write_exact(int fd, const void *buf, size_t count) {
    const unsigned char *ptr = (const unsigned char *)buf;
    size_t total = 0;
    while (total < count) {
        ssize_t w = write(fd, ptr + total, count - total);
        if (w < 0) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        total += (size_t)w;
    }
    return 0;
}

/* Trả về 1 nếu đọc đủ count byte, 0 nếu gặp EOF sạch, -1 nếu lỗi */
static int read_exact_or_eof(int fd, void *buf, size_t count) {
    unsigned char *ptr = (unsigned char *)buf;
    size_t total = 0;
    while (total < count) {
        ssize_t r = read(fd, ptr + total, count - total);
        if (r < 0) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        if (r == 0) {
            return (total == 0) ? 0 : -1;
        }
        total += (size_t)r;
    }
    return 1;
}

int main(void) {
    int n = 0;
    if (scanf("%d", &n) != 1 || n <= 0) {
        return 1;
    }

    long long *arr = (long long *)malloc((size_t)n * sizeof(long long));
    if (arr == NULL) {
        return 1;
    }

    for (int i = 0; i < n; i++) {
        if (scanf("%lld", &arr[i]) != 1) {
            free(arr);
            return 1;
        }
    }

    int p[2];
    if (pipe(p) < 0) {
        free(arr);
        return 1;
    }

    fflush(stdout);
    pid_t pid = fork();
    if (pid < 0) {
        close(p[0]);
        close(p[1]);
        free(arr);
        return 1;
    }

    if (pid == 0) {
        /* Tiến trình con: giải phóng bản sao mảng trước fork, đóng đầu ghi p[1] */
        free(arr);
        close(p[1]);

        long long sum = 0;
        long long val = 0;
        while (1) {
            int st = read_exact_or_eof(p[0], &val, sizeof(val));
            if (st == 0) {
                break; /* Gặp EOF sau khi tiến trình cha đóng p[1] */
            }
            if (st < 0) {
                close(p[0]);
                _exit(1);
            }
            sum += val;
        }

        close(p[0]);
        printf("%lld\n", sum);
        fflush(stdout);
        _exit(0);
    }

    /* Tiến trình cha: đóng đầu đọc p[0] trước khi ghi */
    close(p[0]);

    if (write_exact(p[1], arr, (size_t)n * sizeof(long long)) < 0) {
        close(p[1]);
        free(arr);
        waitpid(pid, NULL, 0);
        return 1;
    }

    /* Đóng p[1] sau khi ghi xong để tiến trình con nhận được EOF */
    close(p[1]);
    free(arr);

    int status = 0;
    if (waitpid(pid, &status, 0) < 0) {
        return 1;
    }

    return 0;
}