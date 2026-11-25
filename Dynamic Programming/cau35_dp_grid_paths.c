#include <stdio.h>

#define MOD 1000000007

int grid[505][505];
long long dp[505][505]; // Mảng toàn cục tự khởi tạo bằng 0 (hàng 0 và cột 0 đóng vai trò biên)

int main() {
    int M, N;
    if (scanf("%d %d", &M, &N) != 2) return 0;

    for (int i = 1; i <= M; i++) {
        for (int j = 1; j <= N; j++) {
            scanf("%d", &grid[i][j]); // Đọc lưới theo chỉ số từ 1 đến M, N
        }
    }

    // Nếu ô xuất phát hoặc ô đích bị chặn bởi vật cản thì không có đường đi
    if (grid[1][1] == 1 || grid[M][N] == 1) {
        printf("0\n");
        return 0;
    }

    dp[1][1] = 1; // Có 1 cách đứng tại ô xuất phát (1, 1)

    // Quy hoạch động Bottom-Up duyệt từng hàng từ trên xuống dưới, trái sang phải
    for (int i = 1; i <= M; i++) {
        for (int j = 1; j <= N; j++) {
            if (i == 1 && j == 1) continue;
            if (grid[i][j] == 1) {
                dp[i][j] = 0; // Ô có vật cản thì số đường đi tới đây bằng 0
            } else {
                // Chỉ có thể đi tới (i, j) từ ô phía trên (i-1, j) hoặc ô bên trái (i, j-1)
                dp[i][j] = (dp[i - 1][j] + dp[i][j - 1]) % MOD;
            }
        }
    }

    printf("%lld\n", dp[M][N]);
    return 0;
}