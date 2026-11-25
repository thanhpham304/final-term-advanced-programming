#include <stdio.h>

#define INF 1000000000 // Giá trị vô cực an toàn, cộng thêm 1 không bị tràn kiểu int

int c[105], dp[100005]; // dp[i]: số đồng xu ít nhất để đổi được số tiền i

int main() {
    int n, S;
    if (scanf("%d %d", &n, &S) != 2) return 0;

    for (int i = 0; i < n; i++) {
        scanf("%d", &c[i]); // Đọc n mệnh giá đồng xu
    }

    dp[0] = 0; // Đổi 0 đồng cần 0 đồng xu
    for (int i = 1; i <= S; i++) {
        dp[i] = INF; // Khởi tạo các mức tiền chưa đổi được là vô cực
    }

    // Quy hoạch động Bottom-Up: tính số xu tối thiểu cho từng mức tiền từ 1 đến S
    for (int i = 1; i <= S; i++) {
        for (int j = 0; j < n; j++) {
            if (c[j] <= i && dp[i - c[j]] + 1 < dp[i]) {
                dp[i] = dp[i - c[j]] + 1; // Chọn thêm đồng xu c[j] nếu giúp giảm số lượng xu
            }
        }
    }

    if (dp[S] >= INF) printf("-1\n"); // Không có cách ghép đúng bằng S
    else printf("%d\n", dp[S]);

    return 0;
}