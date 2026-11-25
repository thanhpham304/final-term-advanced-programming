#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

// Hàm kiểm tra số nguyên tố (số <= 1 trả về 0)
int is_prime(int x) {
    if (x < 2) return 0;
    for (int i = 2; i * i <= x; i++) {
        if (x % i == 0) return 0;
    }
    return 1;
}

int main() {
    int n;
    if (scanf("%d", &n) != 1) return 0;

    int a[10005];
    for (int i = 0; i < n; i++) {
        scanf("%d", &a[i]); // Cha đọc n số nguyên từ stdin
    }

    int p2c[2], c2p[2]; // 2 ống pipe: cha->con (p2c) và con->cha (c2p)
    pipe(p2c);
    pipe(c2p);

    pid_t pid = fork();
    if (pid == 0) {
        // Tiến trình con: đóng đầu ghi p2c và đầu đọc c2p
        close(p2c[1]);
        close(c2p[0]);

        // Đọc đủ n số nguyên từ ống cha gửi sang
        int got = 0, total = n * sizeof(int);
        while (got < total) {
            got += read(p2c[0], (char *)a + got, total - got);
        }
        close(p2c[0]);

        // Tính tổng các số nguyên tố
        long long sum = 0;
        for (int i = 0; i < n; i++) {
            if (is_prime(a[i])) sum += a[i];
        }

        // Gửi kết quả về cho cha và đóng đầu ghi
        write(c2p[1], &sum, sizeof(sum));
        close(c2p[1]);
        _exit(0);
    }

    // Tiến trình cha: đóng đầu đọc p2c và đầu ghi c2p
    close(p2c[0]);
    close(c2p[1]);

    // Gửi mảng sang cho con và đóng ngay đầu ghi
    write(p2c[1], a, n * sizeof(int));
    close(p2c[1]);

    // Nhận tổng từ con và đóng đầu đọc
    long long sum = 0;
    read(c2p[0], &sum, sizeof(sum));
    close(c2p[0]);

    waitpid(pid, NULL, 0); // Đợi con kết thúc để tránh zombie
    printf("%lld\n", sum);
    return 0;
}