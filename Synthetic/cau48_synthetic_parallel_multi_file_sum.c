#include <stdio.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>

char paths[35][256];
long long file_sum[35]; // Mảng lưu tổng giá trị byte của từng tệp

typedef struct {
    int start, end; // Đoạn chỉ số tệp [start, end) được giao cho luồng
} Task;

void *sum_worker(void *arg) {
    Task *t = (Task *)arg;
    unsigned char buf[4096]; // Dùng unsigned char để đọc đúng giá trị byte 0..255 và cả '\0'

    for (int idx = t->start; idx < t->end; idx++) {
        int fd = open(paths[idx], O_RDONLY); // Mở tệp bằng system call open()
        if (fd < 0) {
            file_sum[idx] = -1; // Báo lỗi nếu không mở được tệp
            continue;
        }

        long long sum = 0;
        ssize_t n;
        while ((n = read(fd, buf, sizeof(buf))) > 0) {
            for (ssize_t i = 0; i < n; i++) {
                sum += buf[i];
            }
        }
        close(fd); // Đóng tệp ngay sau khi đọc xong
        file_sum[idx] = (n < 0) ? -1 : sum;
    }
    return NULL;
}

int main() {
    int k, T;
    if (scanf("%d %d", &k, &T) != 2) return 0;

    for (int i = 0; i < k; i++) {
        scanf("%255s", paths[i]);
    }

    int actual_T = (k < T) ? k : T; // Chỉ tạo tối đa min(k, T) luồng
    pthread_t threads[10];
    Task tasks[10];
    int chunk = k / actual_T, rem = k % actual_T;

    // Phân hoạch tĩnh danh sách k tệp cho actual_T luồng chạy song song
    for (int i = 0; i < actual_T; i++) {
        tasks[i].start = i * chunk + (i < rem ? i : rem);
        tasks[i].end = tasks[i].start + chunk + (i < rem ? 1 : 0);
        pthread_create(&threads[i], NULL, sum_worker, &tasks[i]);
    }

    for (int i = 0; i < actual_T; i++) {
        pthread_join(threads[i], NULL); // Chờ tất cả các luồng xử lý xong
    }

    // In đúng k kết quả trên 1 dòng, không dư khoảng trắng cuối dòng
    for (int i = 0; i < k; i++) {
        printf("%lld%s", file_sum[i], (i + 1 < k) ? " " : "\n");
    }

    return 0;
}