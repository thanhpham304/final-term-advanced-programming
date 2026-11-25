#include <stdio.h>

long long p[2005], r[2005]; // r[j]: doanh thu tối đa cho thanh gỗ dài j
int s[2005];                // s[j]: độ dài đoạn cắt đầu tiên tối ưu cho thanh gỗ dài j

int main() {
    int n;
    if (scanf("%d", &n) != 1) return 0;

    for (int i = 1; i <= n; i++) {
        scanf("%lld", &p[i]); // Đọc giá bán của từng đoạn gỗ độ dài i
    }

    r[0] = 0; // Thanh gỗ độ dài 0 có doanh thu bằng 0

    // Thuật toán Extended Bottom-Up Cut-Rod
    for (int j = 1; j <= n; j++) {
        long long q = -1; // Khởi tạo -1 để luôn cập nhật được kể cả khi mọi p[i] = 0
        for (int i = 1; i <= j; i++) {
            // So sánh nghiêm ngặt '>' để giữ đoạn cắt đầu i nhỏ nhất khi có nhiều cách bằng doanh thu
            if (p[i] + r[j - i] > q) {
                q = p[i] + r[j - i];
                s[j] = i;
            }
        }
        r[j] = q;
    }

    printf("%lld\n", r[n]); // Dòng 1: in doanh thu tối đa đạt được

    // Truy vết ngược các đoạn cắt từ thanh gỗ độ dài n cho tới khi hết
    while (n > 0) {
        printf("%d", s[n]);
        n -= s[n];
        printf("%s", (n > 0) ? " " : "\n"); // Chỉ in dấu cách nếu vẫn còn phần gỗ cần cắt tiếp
    }

    return 0;
}