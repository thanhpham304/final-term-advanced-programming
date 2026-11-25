#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    char file_path[135];
    if (scanf("%128s", file_path) != 1) return 0;

    int pfd[2];
    if (pipe(pfd) < 0) return 1;

    pid_t pid = fork();
    if (pid < 0) return 1;

    if (pid == 0) {
        close(pfd[0]); // Đóng đầu đọc ống không dùng ở tiến trình con

        long long sum = 0;
        int fd = open(file_path, O_RDONLY); // Mở tệp bằng system call open()
        if (fd < 0) {
            sum = -1; // Báo lỗi mở tệp về tiến trình cha
        } else {
            char buf[4096];
            ssize_t n;
            long long cur = 0;
            int sign = 1, in_num = 0;

            // Đọc dữ liệu theo từng khối bằng read() và phân tích số nguyên
            while ((n = read(fd, buf, sizeof(buf))) > 0) {
                for (ssize_t i = 0; i < n; i++) {
                    char c = buf[i];
                    if (c == '-') {
                        sign = -1;
                        in_num = 1;
                    } else if (c == '+') {
                        sign = 1;
                        in_num = 1;
                    } else if (c >= '0' && c <= '9') {
                        cur = cur * 10 + (c - '0');
                        in_num = 1;
                    } else if (in_num) {
                        if (sign == 1 && cur > 0) sum += cur; // Chỉ cộng các số nguyên dương (> 0)
                        cur = 0;
                        sign = 1;
                        in_num = 0;
                    }
                }
            }
            if (in_num && sign == 1 && cur > 0) sum += cur; // Xử lý số cuối cùng nếu tệp không kết thúc bằng dấu cách
            close(fd); // Đóng tệp bằng system call close()
        }

        write(pfd[1], &sum, sizeof(sum)); // Gửi kết quả kiểu long long qua pipe
        close(pfd[1]);
        _exit(0);
    } else {
        close(pfd[1]); // Đóng đầu ghi ống không dùng ở tiến trình cha

        long long res = -1;
        read(pfd[0], &res, sizeof(res)); // Đọc kết quả từ tiến trình con
        close(pfd[0]);

        waitpid(pid, NULL, 0); // Thu gom tiến trình con tránh zombie
        printf("%lld\n", res);
    }

    return 0;
}