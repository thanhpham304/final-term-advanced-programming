#include <stdio.h>
#include <pthread.h>

long long buf1[10005], buf2[10005];
long long *dp_prev = buf1, *dp_cur = buf2; // Hai mảng luân phiên lưu trạng thái hàng trước và hàng hiện tại
int cur_w;
long long cur_v;

typedef struct {
    int l, r; // Đoạn trọng lượng [l, r] được giao cho mỗi luồng
} Range;

void *dp_worker(void *arg) {
    Range *rg = (Range *)arg;
    for (int w = rg->l; w <= rg->r; w++) {
        long long best = dp_prev[w]; // Trường hợp không chọn đồ vật hiện tại
        if (w >= cur_w && dp_prev[w - cur_w] + cur_v > best) {
            best = dp_prev[w - cur_w] + cur_v; // Trường hợp chọn đồ vật hiện tại
        }
        dp_cur[w] = best;
    }
    return NULL;
}

int main() {
    int n, W, T;
    if (scanf("%d %d %d", &n, &W, &T) != 3) return 0;

    int num_states = W + 1;
    int actual_T = (T < num_states) ? T : num_states; // Tránh tạo luồng rỗng nếu T > W + 1
    pthread_t threads[10];
    Range ranges[10];

    // Phân hoạch tĩnh dải trọng lượng [0..W] thành actual_T đoạn rời nhau
    int chunk = num_states / actual_T, rem = num_states % actual_T;
    for (int t = 0; t < actual_T; t++) {
        ranges[t].l = t * chunk + (t < rem ? t : rem);
        ranges[t].r = ranges[t].l + chunk + (t < rem ? 1 : 0) - 1;
    }

    for (int i = 0; i < n; i++) {
        scanf("%d %lld", &cur_w, &cur_v);

        // Kích hoạt song song các luồng tính từng đoạn của mảng dp_cur[]
        for (int t = 0; t < actual_T; t++) {
            pthread_create(&threads[t], NULL, dp_worker, &ranges[t]);
        }
        // Đồng bộ hoàn tất toàn bộ hàng i trước khi chuyển sang đồ vật tiếp theo
        for (int t = 0; t < actual_T; t++) {
            pthread_join(threads[t], NULL);
        }

        // Hoán đổi con trỏ dp_prev và dp_cur trong O(1)
        long long *tmp = dp_prev;
        dp_prev = dp_cur;
        dp_cur = tmp;
    }

    printf("%lld\n", dp_prev[W]);
    return 0;
}