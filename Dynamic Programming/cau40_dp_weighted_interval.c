#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int s, f, p;
} Job;

Job jobs[10005];
long long dp[10005]; // dp[i]: tổng lợi nhuận tối đa khi xét i tiến trình đầu tiên

// Hàm so sánh sắp xếp tiến trình tăng dần theo thời điểm kết thúc f
int cmp(const void *a, const void *b) {
    int fa = ((const Job *)a)->f;
    int fb = ((const Job *)b)->f;
    return (fa > fb) - (fa < fb); // Tránh tràn số so với phép trừ trực tiếp
}

int main() {
    int n;
    if (scanf("%d", &n) != 1) return 0;

    for (int i = 1; i <= n; i++) {
        scanf("%d %d %d", &jobs[i].s, &jobs[i].f, &jobs[i].p);
    }

    qsort(jobs + 1, n, sizeof(Job), cmp); // Sắp xếp các tiến trình 1..n theo f tăng dần

    dp[0] = 0; // Không chọn tiến trình nào thì lợi nhuận bằng 0
    for (int i = 1; i <= n; i++) {
        // Tìm kiếm nhị phân tiến trình idx gần nhất (1 <= idx < i) có f[idx] <= s[i]
        int l = 1, r = i - 1, idx = 0;
        while (l <= r) {
            int mid = (l + r) / 2;
            if (jobs[mid].f <= jobs[i].s) {
                idx = mid;   // Lưu kết quả hợp lệ và tiếp tục tìm sát về bên phải
                l = mid + 1;
            } else {
                r = mid - 1;
            }
        }

        // So sánh giữa việc chọn tiến trình i (cộng dp[idx]) và bỏ qua tiến trình i (dp[i - 1])
        long long take = jobs[i].p + dp[idx];
        dp[i] = (take > dp[i - 1]) ? take : dp[i - 1];
    }

    printf("%lld\n", dp[n]);
    return 0;
}