#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int A[100005], B[100005];

int main() {
    int n, k;
    if (scanf("%d %d", &n, &k) != 2) return 0;

    for (int i = 0; i < n; i++) scanf("%d", &A[i]);
    for (int i = 0; i < n; i++) scanf("%d", &B[i]);

    int m = (n < k) ? n : k; // Sinh tối đa min(n, k) tiến trình con
    int fd[10][2];           // Mảng ống pipe riêng biệt cho từng con (k <= 8)
    pid_t pids[10];

    for (int i = 0; i < m; i++) {
        pipe(fd[i]); // Tạo đường ống riêng cho tiến trình con thứ i
        pids[i] = fork();
        if (pids[i] == 0) {
            // Đóng toàn bộ các đầu đọc ống thừa mà con thứ i kế thừa từ cha
            for (int j = 0; j <= i; j++) close(fd[j][0]);

            // Phân hoạch tĩnh đoạn chỉ số [L, R) cho con thứ i
            int L = i * n / m;
            int R = (i + 1) * n / m;
            long long part = 0;
            for (int j = L; j < R; j++) {
                part += 1LL * A[j] * B[j]; // Ép kiểu long long trước khi nhân để tránh tràn số
            }

            write(fd[i][1], &part, sizeof(part)); // Gửi tổng cục bộ về cha qua ống riêng
            close(fd[i][1]);
            _exit(0); // Thoát ngay để không sinh thêm tiến trình cháu
        }
        close(fd[i][1]); // Cha không ghi vào ống i nên đóng ngay đầu ghi
    }

    long long total = 0, part;
    for (int i = 0; i < m; i++) {
        read(fd[i][0], &part, sizeof(part)); // Đọc kết quả từ ống riêng của con thứ i
        close(fd[i][0]);
        total += part;
        waitpid(pids[i], NULL, 0); // Thu gom tiến trình con thứ i để tránh zombie
    }

    printf("%lld\n", total);
    return 0;
}