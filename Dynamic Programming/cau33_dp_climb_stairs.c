#include <stdio.h>

long long cost[100005], dp[100005]; // Mảng toàn cục tránh tràn stack với n <= 10^5

int main() {
    int n;
    if (scanf("%d", &n) != 1) return 0;

    for (int i = 1; i <= n; i++) {
        scanf("%lld", &cost[i]); // Đọc chi phí từng bậc từ 1 đến n
    }

    dp[0] = 0;       // Đứng ở mặt đất (bậc 0) không mất chi phí
    dp[1] = cost[1]; // Chỉ có một cách bước lên bậc 1 là từ bậc 0

    // Quy hoạch động Bottom-Up: để tới bậc i chỉ có thể nhảy từ bậc i-1 hoặc i-2
    for (int i = 2; i <= n; i++) {
        long long prev_min = (dp[i - 1] < dp[i - 2]) ? dp[i - 1] : dp[i - 2];
        dp[i] = prev_min + cost[i];
    }

    printf("%lld\n", dp[n]); // In chi phí nhỏ nhất để đặt chân lên đúng bậc n
    return 0;
}