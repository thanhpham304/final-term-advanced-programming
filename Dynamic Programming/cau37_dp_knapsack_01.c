#include <stdio.h>

long long dp[10005]; // dp[j]: tổng giá trị lớn nhất với sức chứa j (khởi tạo toàn cục bằng 0)

int main() {
    int n, W;
    if (scanf("%d %d", &n, &W) != 2) return 0;

    for (int i = 0; i < n; i++) {
        int w;
        long long v;
        scanf("%d %lld", &w, &v); // Đọc lần lượt từng đồ vật (không cần lưu mảng w[], v[])

        // Duyệt ngược j từ W về w để mỗi đồ vật chỉ được chọn tối đa 1 lần (0/1 Knapsack)
        for (int j = W; j >= w; j--) {
            if (dp[j - w] + v > dp[j]) {
                dp[j] = dp[j - w] + v;
            }
        }
    }

    printf("%lld\n", dp[W]);
    return 0;
}