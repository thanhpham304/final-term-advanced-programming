#include <stdio.h>
#include <pthread.h>

int M, K, N, T;
int A[105][105], B[105][105];
long long C[105][105]; // Ma trận tích khởi tạo sẵn bằng 0 ở vùng nhớ toàn cục

void *worker(void *arg) {
    int tid = *(int *)arg;
    int m = (M < T) ? M : T; // Số luồng thực tế được tạo

    // Phân hoạch đều dải hàng [start, end) của ma trận C cho luồng tid
    int start = tid * M / m;
    int end = (tid + 1) * M / m;

    // Duyệt vòng lặp thứ tự i-k-j để truy cập tuần tự bộ nhớ đệm (cache-friendly)
    for (int i = start; i < end; i++) {
        for (int k = 0; k < K; k++) {
            long long a_ik = A[i][k];
            for (int j = 0; j < N; j++) {
                C[i][j] += a_ik * B[k][j];
            }
        }
    }
    return NULL;
}

int main() {
    if (scanf("%d %d %d %d", &M, &K, &N, &T) != 4) return 0;

    for (int i = 0; i < M; i++)
        for (int j = 0; j < K; j++) scanf("%d", &A[i][j]);

    for (int i = 0; i < K; i++)
        for (int j = 0; j < N; j++) scanf("%d", &B[i][j]);

    int m = (M < T) ? M : T; // Tạo tối đa min(M, T) luồng để không sinh luồng thừa khi T > M
    pthread_t tids[20];
    int ids[20]; // Mảng lưu chỉ số riêng (0..m-1) cho từng luồng

    for (int i = 0; i < m; i++) {
        ids[i] = i;
        pthread_create(&tids[i], NULL, worker, &ids[i]);
    }

    for (int i = 0; i < m; i++) {
        pthread_join(tids[i], NULL); // Chờ toàn bộ m luồng tính xong các dải hàng
    }

    // In ma trận kết quả C gồm M dòng, mỗi dòng N số, không dư khoảng trắng cuối dòng
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            printf("%lld%s", C[i][j], (j + 1 < N) ? " " : "\n");
        }
    }
    return 0;
}