#include <stdio.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

typedef struct {
    const char *path;
    off_t start;
    off_t len;
    unsigned char target;
    long long count;
} Task;

void *count_worker(void *arg) {
    Task *t = (Task *)arg;
    t->count = 0;

    int fd = open(t->path, O_RDONLY); // Mỗi luồng mở file descriptor riêng để tránh race condition
    if (fd < 0) {
        t->count = -1;
        return NULL;
    }

    if (lseek(fd, t->start, SEEK_SET) < 0) { // Định vị con trỏ tệp tới đầu đoạn được giao
        close(fd);
        t->count = -1;
        return NULL;
    }

    unsigned char buf[4096];
    off_t left = t->len;
    while (left > 0) {
        ssize_t to_read = (left > (off_t)sizeof(buf)) ? (ssize_t)sizeof(buf) : (ssize_t)left;
        ssize_t n = read(fd, buf, to_read); // Đọc đúng số byte thuộc phạm vi của luồng
        if (n <= 0) break;

        for (ssize_t i = 0; i < n; i++) {
            if (buf[i] == t->target) t->count++; // So sánh unsigned char với target_byte
        }
        left -= n;
    }

    close(fd); // Đóng file descriptor riêng của luồng
    return NULL;
}

int main() {
    char file_path[135];
    int target_byte, T;
    if (scanf("%128s %d %d", file_path, &target_byte, &T) != 3) return 0;

    struct stat st;
    int test_fd = open(file_path, O_RDONLY); // Kiểm tra quyền mở tệp và thông tin kích thước qua stat()
    if (stat(file_path, &st) < 0 || test_fd < 0) {
        if (test_fd >= 0) close(test_fd);
        printf("-1\n");
        return 0;
    }
    close(test_fd);

    off_t size = st.st_size;
    if (size == 0) {
        printf("0\n");
        return 0;
    }

    pthread_t threads[10];
    Task tasks[10];
    off_t chunk = size / T, rem = size % T;

    // Chia [0, size) thành T đoạn liên tiếp và khởi tạo T luồng xử lý song song
    for (int i = 0; i < T; i++) {
        tasks[i].path = file_path;
        tasks[i].start = i * chunk + (i < rem ? i : rem);
        tasks[i].len = chunk + (i < rem ? 1 : 0);
        tasks[i].target = (unsigned char)target_byte;
        pthread_create(&threads[i], NULL, count_worker, &tasks[i]);
    }

    long long total = 0;
    for (int i = 0; i < T; i++) {
        pthread_join(threads[i], NULL);
        if (tasks[i].count < 0) {
            printf("-1\n");
            return 0;
        }
        total += tasks[i].count; // Cộng dồn kết quả cục bộ từ từng luồng
    }

    printf("%lld\n", total);
    return 0;
}