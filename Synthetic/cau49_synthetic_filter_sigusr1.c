#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>

volatile sig_atomic_t got_sig = 0; // Cờ trạng thái nhận tín hiệu SIGUSR1 từ tiến trình con

void sigusr1_handler(int sig) {
    (void)sig;
    got_sig = 1;
}

int main() {
    char src_path[135], dst_path[135];
    if (scanf("%128s %128s", src_path, dst_path) != 2) return 0;

    // Cài đặt signal handler TRƯỚC khi fork() để tránh mất tín hiệu
    struct sigaction sa;
    sa.sa_handler = sigusr1_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGUSR1, &sa, NULL);

    pid_t pid = fork();
    if (pid < 0) return 1;

    if (pid == 0) {
        int fd_in = open(src_path, O_RDONLY);
        if (fd_in < 0) _exit(1); // Thoát mã lỗi 1 và không gửi tín hiệu nếu lỗi mở tệp nguồn

        int fd_out = open(dst_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd_out < 0) {
            close(fd_in);
            _exit(1);
        }

        unsigned char in_buf[4096], out_buf[4096];
        ssize_t n;
        while ((n = read(fd_in, in_buf, sizeof(in_buf))) > 0) {
            ssize_t out_len = 0;
            for (ssize_t i = 0; i < n; i++) {
                unsigned char b = in_buf[i];
                // Chỉ giữ lại ký tự ASCII in được (32..126) hoặc xuống dòng '\n' (10)
                if ((b >= 32 && b <= 126) || b == '\n') {
                    out_buf[out_len++] = b;
                }
            }
            if (out_len > 0) {
                write(fd_out, out_buf, out_len); // Ghi dữ liệu đã lọc xuống tệp đích
            }
        }

        close(fd_in);
        close(fd_out);

        kill(getppid(), SIGUSR1); // Gửi tín hiệu báo hoàn tất về cho tiến trình cha
        _exit(0);
    } else {
        int status;
        waitpid(pid, &status, 0); // Thu gom tiến trình con

        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            printf("-1 -1\n"); // Con thoát khác 0 (lỗi mở tệp nguồn)
            return 0;
        }

        struct stat st;
        if (stat(dst_path, &st) < 0) {
            printf("-1 -1\n");
            return 0;
        }

        printf("%d %lld\n", (int)got_sig, (long long)st.st_size);
    }

    return 0;
}