#include <stdio.h>
#include <pthread.h>

int N, B_tile, T;
int A[205][205], B[205][205];
long long C[205][205]; // Ma trận tích khởi tạo sẵn bằng 0 ở vùng nhớ toàn cục

void *worker(void *arg) {
    int tid = *(int *)arg;
    // Phân hoạch đều các hàng [start, end) của ma trận C cho T luồng
    int start = tid * N / T;
    int end = (tid + 1) * N / T;

    // Chia chiều k và j thành từng khối kích thước B_tile x B_tile (Cache Blocking)
    for (int kk = 0; kk < N; kk += B_tile) {
        int k_end = (kk + B_tile < N) ? (kk + B_tile) : N; // Xử lý phần dư khi N không chia hết cho B_tile
        for (int jj = 0; jj < N; jj += B_tile) {
            int j_end = (jj + B_tile < N) ? (jj + B_tile) : N;

            // Tính tích trên từng khối con theo thứ tự i-k-j để tận dụng bộ nhớ đệm L1
            for (int i = start; i < end; i++) {
                for (int k = kk; k < k_end; k++) {
                    long long a_ik = A[i][k];
                    for (int j = jj; j < j_end; j++) {
                        C[i][j] += a_ik * B[k][j];
                    }
                }
            }
        }
    }
    return NULL;
}

int main() {
    if (scanf("%d %d %d", &N, &B_tile, &T) != 3) return 0;

    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) scanf("%d", &A[i][j]);

    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) scanf("%d", &B[i][j]);

    pthread_t tids[20];
    int ids[20]; // Mảng lưu chỉ số riêng (0..T-1) truyền cho từng luồng
    for (int i = 0; i < T; i++) {
        ids[i] = i;
        pthread_create(&tids[i], NULL, worker, &ids[i]);
    }

    for (int i = 0; i < T; i++) {
        pthread_join(tids[i], NULL); // Chờ toàn bộ T luồng tính xong các dải hàng của C
    }

    // Tính và in tổng các phần tử trên mỗi hàng của ma trận C trên 1 dòng
    for (int i = 0; i < N; i++) {
        long long row_sum = 0;
        for (int j = 0; j < N; j++) row_sum += C[i][j];
        printf("%lld%s", row_sum, (i + 1 < N) ? " " : "\n");
    }
    return 0;
}