#include <stdio.h>
#include <pthread.h>

int n, T;
int A[100005], B[100005];
long long partial[20]; // Mảng lưu tổng cục bộ riêng của từng luồng (1 <= T <= 16)

void *worker(void *arg) {
    int tid = *(int *)arg;
    int m = (n < T) ? n : T; // Số luồng thực tế được khởi tạo

    // Phân hoạch đều đoạn chỉ số [start, end) cho luồng tid
    int start = tid * n / m;
    int end = (tid + 1) * n / m;

    long long local_sum = 0; // Tích lũy vào biến cục bộ để tránh xung đột bộ nhớ
    for (int i = start; i < end; i++) {
        local_sum += 1LL * A[i] * B[i]; // Ép kiểu long long trước khi nhân để tránh tràn số
    }

    partial[tid] = local_sum; // Ghi kết quả đúng 1 lần vào vị trí riêng của luồng
    return NULL;
}

int main() {
    if (scanf("%d %d", &n, &T) != 2) return 0;
    for (int i = 0; i < n; i++) scanf("%d", &A[i]);
    for (int i = 0; i < n; i++) scanf("%d", &B[i]);

    int m = (n < T) ? n : T; // Tạo tối đa min(n, T) luồng để không sinh luồng thừa
    pthread_t tids[20];
    int ids[20]; // Mảng lưu chỉ số riêng (0..m-1) truyền cho từng luồng

    for (int i = 0; i < m; i++) {
        ids[i] = i;
        pthread_create(&tids[i], NULL, worker, &ids[i]);
    }

    long long total = 0;
    for (int i = 0; i < m; i++) {
        pthread_join(tids[i], NULL); // Chờ luồng i hoàn thành
        total += partial[i];         // Cộng dồn tổng cục bộ vào kết quả cuối
    }

    printf("%lld\n", total);
    return 0;
}