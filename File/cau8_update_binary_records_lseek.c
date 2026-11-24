#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

struct Record {
    int32_t id;
    int32_t score;
};

static int read_exact(int fd, void *buf, size_t count) {
    unsigned char *ptr = (unsigned char *)buf;
    size_t total = 0;
    while (total < count) {
        ssize_t n = read(fd, ptr + total, count - total);
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        if (n == 0) {
            return -1;
        }
        total += (size_t)n;
    }
    return 0;
}

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

int main(void) {
    char file_path[512];
    long long q = 0;

    if (scanf("%511s %lld", file_path, &q) != 2 || q < 0) {
        printf("-1\n");
        return 0;
    }

    int fd = open(file_path, O_RDWR);
    if (fd < 0) {
        printf("-1\n");
        return 0;
    }

    struct stat st;
    if (fstat(fd, &st) < 0 || st.st_size < 0 ||
        (st.st_size % (off_t)sizeof(struct Record)) != 0) {
        close(fd);
        printf("-1\n");
        return 0;
    }

    long long n_records = (long long)(st.st_size / (off_t)sizeof(struct Record));

    for (long long i = 0; i < q; i++) {
        long long idx = 0;
        long long delta = 0;
        if (scanf("%lld %lld", &idx, &delta) != 2) {
            close(fd);
            printf("-1\n");
            return 0;
        }

        if (idx < 0 || idx >= n_records) {
            continue;
        }

        off_t offset = (off_t)(idx * (long long)sizeof(struct Record));

        if (lseek(fd, offset, SEEK_SET) == (off_t)-1) {
            close(fd);
            printf("-1\n");
            return 0;
        }

        struct Record rec;
        if (read_exact(fd, &rec, sizeof(rec)) < 0) {
            close(fd);
            printf("-1\n");
            return 0;
        }

        rec.score = (int32_t)((long long)rec.score + delta);

        if (lseek(fd, offset, SEEK_SET) == (off_t)-1) {
            close(fd);
            printf("-1\n");
            return 0;
        }

        if (write_exact(fd, &rec, sizeof(rec)) < 0) {
            close(fd);
            printf("-1\n");
            return 0;
        }
    }

    if (lseek(fd, 0, SEEK_SET) == (off_t)-1) {
        close(fd);
        printf("-1\n");
        return 0;
    }

    long long total_score = 0;
    for (long long i = 0; i < n_records; i++) {
        struct Record rec;
        if (read_exact(fd, &rec, sizeof(rec)) < 0) {
            close(fd);
            printf("-1\n");
            return 0;
        }
        total_score += (long long)rec.score;
    }

    close(fd);
    printf("%lld\n", total_score);
    return 0;
}