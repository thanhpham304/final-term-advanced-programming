#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int k;
    if (scanf("%d", &k) != 1) return 0;

    int fd[2];
    pipe(fd); // Tạo đường ống: fd[0] để đọc, fd[1] để ghi

    pid_t pids[20]; // Lưu PID của k tiến trình con (1 <= k <= 16)

    for (int i = 1; i <= k; i++) {
        pids[i] = fork(); // Cha tạo tiến trình con thứ i
        if (pids[i] == 0) {
            close(fd[0]); // Con không đọc nên đóng đầu đọc
            long long val = 1LL * i * i * i; // Tính i^3 kiểu long long
            write(fd[1], &val, sizeof(val)); // Gửi 8 byte kết quả vào ống
            close(fd[1]); // Đóng đầu ghi sau khi gửi xong
            _exit(0); // Thoát ngay để không chạy tiếp vòng lặp (tránh tạo tiến trình cháu)
        }
    }

    close(fd[1]); // Cha không ghi nên đóng đầu ghi

    long long sum = 0, val;
    for (int i = 1; i <= k; i++) {
        read(fd[0], &val, sizeof(val)); // Cha đọc lần lượt từng kết quả từ ống
        sum += val;
    }
    close(fd[0]); // Đóng đầu đọc sau khi nhận đủ k giá trị

    for (int i = 1; i <= k; i++) {
        waitpid(pids[i], NULL, 0); // Thu gom toàn bộ k tiến trình con để tránh zombie
    }

    printf("%lld\n", sum);
    return 0;
}