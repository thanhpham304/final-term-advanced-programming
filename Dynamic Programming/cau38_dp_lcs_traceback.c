#include <stdio.h>
#include <string.h>

char X[1005], Y[1005], res[1005];
int dp[1005][1005]; // Bảng quy hoạch động lưu độ dài LCS (khởi tạo toàn cục bằng 0)

int main() {
    if (scanf("%s %s", X, Y) != 2) return 0;

    int m = strlen(X);
    int n = strlen(Y);

    // Quy hoạch động Bottom-Up lập bảng độ dài LCS
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (X[i - 1] == Y[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1; // Khớp ký tự: nối dài LCS thêm 1
            } else {
                // Không khớp: lấy độ dài lớn hơn giữa bỏ X[i-1] hoặc bỏ Y[j-1]
                dp[i][j] = (dp[i - 1][j] > dp[i][j - 1]) ? dp[i - 1][j] : dp[i][j - 1];
            }
        }
    }

    int len = dp[m][n];
    printf("%d\n", len); // Dòng 1: in độ dài LCS

    if (len == 0) {
        printf("EMPTY\n"); // Không có ký tự chung
        return 0;
    }

    // Truy vết ngược từ (m, n) về (0, 0) và điền ký tự từ cuối về đầu mảng res
    res[len] = '\0';
    int i = m, j = n, idx = len - 1;
    while (i > 0 && j > 0) {
        if (X[i - 1] == Y[j - 1]) {
            res[idx--] = X[i - 1]; // Ghi ký tự chung vào vị trí hiện tại và lùi chéo
            i--;
            j--;
        } else if (dp[i - 1][j] > dp[i][j - 1]) {
            i--; // Ưu tiên đi lên khi ô trên lớn hơn hẳn ô trái
        } else {
            j--; // Ngược lại đi sang trái
        }
    }

    printf("%s\n", res); // Dòng 2: in chuỗi LCS truy vết được
    return 0;
}