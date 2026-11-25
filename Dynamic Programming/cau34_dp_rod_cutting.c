#include <stdio.h>

long long p[2005], dp[2005]; // dp[i]: doanh thu tối đa cho thanh gỗ dài i

int main() {
    int n;
    if (scanf("%d", &n) != 1) return 0;

    for (int i = 1; i <= n; i++) {
        scanf("%lld", &p[i]); // Đọc giá bán của từng đoạn gỗ độ dài i
    }

    dp[0] = 0; // Thanh gỗ độ dài 0 có doanh thu bằng 0

    // Quy hoạch động Bottom-Up: tính doanh thu tối ưu dần từ độ dài 1 lên n
    for (int i = 1; i <= n; i++) {
        long long best = 0;
        for (int j = 1; j <= i; j++) { // Thử cắt một đoạn đầu dài j (1 <= j <= i)
            if (p[j] + dp[i - j] > best) {
                best = p[j] + dp[i - j];
            }
        }
        dp[i] = best;
    }

    printf("%lld\n", dp[n]);
    return 0;
}