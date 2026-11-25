#include <stdio.h>
#include <pthread.h>

long long L, R, d;
int T;
long long counts[20]; // Mảng lưu kết quả đếm riêng biệt của từng luồng (1 <= T <= 16)

void *worker(void *arg) {
    int tid = *(int *)arg;
    long long N = R - L + 1;

    // Phân hoạch đều đoạn [L, R] thành T phần liên tiếp cho luồng tid
    long long start = L + tid * N / T;
    long long end = L + (tid + 1) * N / T - 1;

    long long local_cnt = 0; // Biến đếm cục bộ giúp tránh nghẽn cổ chai đồng bộ
    for (long long x = start; x <= end; x++) {
        if (x % 2 != 0 && x % d == 0) {
            local_cnt++;
        }
    }

    counts[tid] = local_cnt; // Chỉ ghi đúng 1 lần vào ô nhớ riêng của luồng khi xong vòng lặp
    return NULL;
}

int main() {
    if (scanf("%lld %lld %lld %d", &L, &R, &d, &T) != 4) return 0;

    pthread_t tids[20];
    int ids[20]; // Mảng lưu chỉ số riêng (0..T-1) cho từng luồng

    for (int i = 0; i < T; i++) {
        ids[i] = i;
        pthread_create(&tids[i], NULL, worker, &ids[i]);
    }

    long long total = 0;
    for (int i = 0; i < T; i++) {
        pthread_join(tids[i], NULL); // Chờ luồng i hoàn tất
        total += counts[i];          // Cộng dồn kết quả cục bộ vào tổng chung
    }

    printf("%lld\n", total);
    return 0;
}