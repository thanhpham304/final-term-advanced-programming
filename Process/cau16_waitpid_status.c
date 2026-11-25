#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

int main() {
    int k, c[20];
    if (scanf("%d", &k) != 1) return 0;
    for (int i = 0; i < k; i++) scanf("%d", &c[i]);

    pid_t pids[20];
    for (int i = 0; i < k; i++) {
        pids[i] = fork(); // Cha tạo tiến trình con thứ i (mô hình Fan-out)
        if (pids[i] == 0) {
            if (c[i] >= 0) {
                _exit(c[i] % 200); // Thoát bình thường với mã trong khoảng 0..199
            } else {
                kill(getpid(), SIGKILL); // Tự kết thúc bằng tín hiệu SIGKILL
                _exit(1);
            }
        }
    }

    int norm_cnt = 0, sum_code = 0, kill_cnt = 0;
    for (int i = 0; i < k; i++) {
        int status;
        waitpid(pids[i], &status, 0); // Thu gom từng tiến trình con và lấy trạng thái status

        if (WIFEXITED(status)) {
            norm_cnt++;                      // Con thoát bình thường
            sum_code += WEXITSTATUS(status); // Lấy mã thoát (0..255) và cộng dồn
        } else if (WIFSIGNALED(status) && WTERMSIG(status) == SIGKILL) {
            kill_cnt++;                      // Con bị giết bởi tín hiệu SIGKILL
        }
    }

    printf("%d %d %d\n", norm_cnt, sum_code, kill_cnt);
    return 0;
}