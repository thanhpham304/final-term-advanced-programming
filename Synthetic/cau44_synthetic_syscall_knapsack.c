#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

long long dp[10005]; // dp[j]: tổng giá trị lớn nhất với sức chứa j
int fd_in;
char in_buf[4096];
ssize_t in_pos = 0, in_len = 0;

// Đọc lần lượt từng số nguyên không âm từ tệp nguồn qua buffer của read()
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

int main() {
    char src_path[256], dst_path[256];
    if (scanf("%255s %255s", src_path, dst_path) != 2) return 0;

    fd_in = open(src_path, O_RDONLY); // Mở tệp nguồn bằng system call open()
    if (fd_in < 0) {
        printf("-1\n"); // Báo lỗi nếu không mở được tệp nguồn
        return 0;
    }

    int n = (int)read_ll();
    int W = (int)read_ll();

    // Giải bài toán Balo 0/1 bằng Quy hoạch động Bottom-Up
    for (int i = 0; i < n; i++) {
        int w = (int)read_ll();
        long long v = read_ll();
        for (int j = W; j >= w; j--) {
            if (dp[j - w] + v > dp[j]) {
                dp[j] = dp[j - w] + v;
            }
        }
    }
    close(fd_in); // Đóng tệp nguồn

    // Mở tệp đích với cờ tạo mới / ghi đè và quyền 0644
    int fd_out = open(dst_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_out >= 0) {
        char out_buf[32];
        int len = snprintf(out_buf, sizeof(out_buf), "%lld\n", dp[W]);
        write(fd_out, out_buf, len); // Ghi kết quả "<ans>\n" bằng write()
        close(fd_out);
    }

    printf("%lld\n", dp[W]); // In kết quả ra stdout
    return 0;
}