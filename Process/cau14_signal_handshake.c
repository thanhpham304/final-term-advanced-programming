#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

// Các biến chia sẻ với signal handler bắt buộc dùng volatile sig_atomic_t
volatile sig_atomic_t cnt = 0;
volatile sig_atomic_t got_sig1 = 0, got_sig2 = 0;

void on_sig1(int sig) {
    (void)sig;
    cnt++;        // Cha tăng biến đếm mỗi khi nhận được SIGUSR1 thật
    got_sig1 = 1; // Bật cờ báo đã nhận SIGUSR1
}

void on_sig2(int sig) {
    (void)sig;
    got_sig2 = 1; // Bật cờ báo con đã nhận xác nhận SIGUSR2 từ cha
}

int main() {
    int m;
    if (scanf("%d", &m) != 1) return 0;

    // Đăng ký bộ xử lý tín hiệu cho SIGUSR1 và SIGUSR2 trước khi fork()
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sa.sa_handler = on_sig1;
    sigaction(SIGUSR1, &sa, NULL);

    sa.sa_handler = on_sig2;
    sigaction(SIGUSR2, &sa, NULL);

    // Chặn tạm thời SIGUSR1 và SIGUSR2 trước khi fork() để tránh race condition
    sigset_t mask, empty;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);
    sigprocmask(SIG_BLOCK, &mask, NULL);
    sigemptyset(&empty); // Mặt nạ rỗng dùng cho sigsuspend()

    pid_t pid = fork();
    if (pid == 0) {
        // Tiến trình con: gửi m lần SIGUSR1 và chờ xác nhận SIGUSR2 sau mỗi lần
        for (int i = 0; i < m; i++) {
            got_sig2 = 0;
            kill(getppid(), SIGUSR1); // Gửi tín hiệu tới cha
            while (!got_sig2) {
                sigsuspend(&empty);   // Mở chặn tạm thời và ngủ chờ SIGUSR2 từ cha
            }
        }
        _exit(0);
    }

    // Tiến trình cha: nhận đủ m lần SIGUSR1 và gửi lại SIGUSR2 xác nhận
    for (int i = 0; i < m; i++) {
        got_sig1 = 0;
        while (!got_sig1) {
            sigsuspend(&empty); // Mở chặn tạm thời và ngủ chờ SIGUSR1 từ con
        }
        kill(pid, SIGUSR2);     // Gửi xác nhận lại cho con để con gửi tiếp
    }

    waitpid(pid, NULL, 0); // Thu gom tiến trình con
    printf("%d\n", (int)cnt);
    return 0;
}