#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

typedef struct {
    long long mn;
    long long mx;
} MinMax;

long long a[100005];

int main() {
    int n, k;
    if (scanf("%d %d", &n, &k) != 2) return 0;

    for (int i = 0; i < n; i++) {
        scanf("%lld", &a[i]);
    }

    int c = (k < n) ? k : n; // Giới hạn số tiến trình con tối đa là min(n, k)
    int pfd[10][2];
    pid_t pids[10];
    int chunk = n / c, rem = n % c;

    for (int i = 0; i < c; i++) {
        if (pipe(pfd[i]) < 0) return 1; // Tạo ống pipe riêng cho tiến trình con thứ i

        int start = i * chunk + (i < rem ? i : rem);
        int end = start + chunk + (i < rem ? 1 : 0);

        pids[i] = fork();
        if (pids[i] < 0) return 1;

        if (pids[i] == 0) {
            close(pfd[i][0]); // Đóng đầu đọc ống không dùng ở tiến trình con

            MinMax local = {a[start], a[start]};
            for (int j = start + 1; j < end; j++) {
                if (a[j] < local.mn) local.mn = a[j];
                if (a[j] > local.mx) local.mx = a[j];
            }

            write(pfd[i][1], &local, sizeof(local)); // Gửi trọn gói struct (mn, mx) về cha
            close(pfd[i][1]);
            _exit(0);
        }

        close(pfd[i][1]); // Tiến trình cha đóng ngay đầu ghi ống của con thứ i
    }

    long long global_min = a[0], global_max = a[0];
    for (int i = 0; i < c; i++) {
        MinMax res;
        if (read(pfd[i][0], &res, sizeof(res)) == sizeof(res)) {
            if (res.mn < global_min) global_min = res.mn;
            if (res.mx > global_max) global_max = res.mx;
        }
        close(pfd[i][0]); // Đóng đầu đọc sau khi nhận xong dữ liệu
    }

    for (int i = 0; i < c; i++) {
        waitpid(pids[i], NULL, 0); // Thu gom toàn bộ tiến trình con tránh zombie
    }

    printf("%lld %lld\n", global_min, global_max);
    return 0;
}