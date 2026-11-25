#include <stdio.h>
#include <pthread.h>

int n, T;
int a[100005];
long long partial[20]; // Mảng lưu tổng bình phương cục bộ của từng luồng (1 <= T <= 16)

void *worker(void *arg) {
    int tid = *(int *)arg;
    int base = n / T; // Số phần tử cơ bản mỗi luồng nhận được
    int rem = n % T;  // Phần dư chia đều cho rem luồng đầu tiên (mỗi luồng thêm 1 phần tử)

    // Xác định đoạn chỉ số [start, end) của luồng tid
    int start = tid * base + (tid < rem ? tid : rem);
    int end = start + base + (tid < rem ? 1 : 0);

    long long local_sum = 0; // Tích lũy vào biến cục bộ để tránh xung đột bộ nhớ
    for (int i = start; i < end; i++) {
        local_sum += 1LL * a[i] * a[i]; // Ép kiểu long long trước khi nhân bình phương
    }

    partial[tid] = local_sum; // Ghi kết quả đúng 1 lần vào mảng riêng của luồng
    return NULL;
}

int main() {
    if (scanf("%d %d", &n, &T) != 2) return 0;
    for (int i = 0; i < n; i++) scanf("%d", &a[i]);

    pthread_t tids[20];
    int ids[20]; // Mảng lưu chỉ số riêng (0..T-1) truyền cho từng luồng

    for (int i = 0; i < T; i++) {
        ids[i] = i;
        pthread_create(&tids[i], NULL, worker, &ids[i]);
    }

    long long total = 0;
    for (int i = 0; i < T; i++) {
        pthread_join(tids[i], NULL); // Đợi luồng i chạy xong
        total += partial[i];         // Cộng dồn tổng cục bộ vào kết quả cuối
    }

    printf("%lld\n", total);
    return 0;
}