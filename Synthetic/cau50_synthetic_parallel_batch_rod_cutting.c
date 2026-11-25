#include <stdio.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

int Q;
int n_arr[105];
long long p_arr[105][505];
long long ans[105];

int fd_in;
char in_buf[4096];
ssize_t in_pos = 0, in_len = 0;

// Đọc từng số nguyên không âm từ tệp nguồn thông qua bộ đệm của read()
long long read_ll() {
    long long val = 0;
    int started = 0;
    while (1) {
        if (in_pos >= in_len) {
            in_len = read(fd_in, in_buf, sizeof(in_buf));
            in_pos = 0;
            if (in_len <= 0) break;
        }
        char c = in_buf[in_pos++];
        if (c >= '0' && c <= '9') {
            val = val * 10 + (c - '0');
            started = 1;
        } else if (started) {
            break;
        }
    }
    return val;
}

typedef struct {
    int start, end; // Đoạn chỉ số bộ dữ liệu [start, end) được phân công cho luồng
} Task;

void *solve_worker(void *arg) {
    Task *t = (Task *)arg;
    long long dp[505]; // Cấp phát mảng DP cục bộ trên từng luồng để tránh tranh chấp dữ liệu

    for (int q = t->start; q < t->end; q++) {
        int n = n_arr[q];
        dp[0] = 0;

        // Quy hoạch động Tabulation O(n^2) tìm doanh thu tối đa cho thanh gỗ dài n
        for (int j = 1; j <= n; j++) {
            long long best = 0;
            for (int i = 1; i <= j; i++) {
                long long cur = p_arr[q][i] + dp[j - i];
                if (cur > best) best = cur;
            }
            dp[j] = best;
        }
        ans[q] = dp[n];
    }
    return NULL;
}

int main() {
    char src_path[256], dst_path[256];
    int T;
    if (scanf("%255s %255s %d", src_path, dst_path, &T) != 3) return 0;

    fd_in = open(src_path, O_RDONLY); // Mở tệp nguồn bằng system call open()
    if (fd_in < 0) {
        printf("-1\n"); // Báo lỗi nếu không mở được tệp nguồn
        return 0;
    }

    Q = (int)read_ll();
    for (int q = 0; q < Q; q++) {
        n_arr[q] = (int)read_ll();
        for (int i = 1; i <= n_arr[q]; i++) {
            p_arr[q][i] = read_ll();
        }
    }
    close(fd_in); // Đóng tệp nguồn sau khi nạp toàn bộ Q bộ dữ liệu

    int actual_T = (Q < T) ? Q : T; // Giới hạn số luồng tối đa là min(Q, T)
    pthread_t threads[10];
    Task tasks[10];
    int chunk = Q / actual_T, rem = Q % actual_T;

    // Phân hoạch tĩnh Q bộ dữ liệu cho actual_T luồng tính toán song song
    for (int i = 0; i < actual_T; i++) {
        tasks[i].start = i * chunk + (i < rem ? i : rem);
        tasks[i].end = tasks[i].start + chunk + (i < rem ? 1 : 0);
        pthread_create(&threads[i], NULL, solve_worker, &tasks[i]);
    }

    for (int i = 0; i < actual_T; i++) {
        pthread_join(threads[i], NULL); // Chờ tất cả các luồng hoàn tất tính toán
    }

    // Ghi lần lượt Q kết quả ra tệp đích bằng open(), write(), close()
    int fd_out = open(dst_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    long long total_sum = 0;
    for (int q = 0; q < Q; q++) {
        total_sum += ans[q];
        if (fd_out >= 0) {
            char line[32];
            int len = snprintf(line, sizeof(line), "%lld\n", ans[q]);
            write(fd_out, line, len);
        }
    }
    if (fd_out >= 0) close(fd_out);

    printf("%lld\n", total_sum); // In tổng doanh thu tối đa của cả Q bộ dữ liệu ra stdout
    return 0;
}