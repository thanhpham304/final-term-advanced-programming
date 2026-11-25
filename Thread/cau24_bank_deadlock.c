#include <stdio.h>
#include <pthread.h>

typedef struct {
    int u, v, amount;
} Tx;

int M, T, Q;
long long bal[70];               // Số dư các tài khoản (dùng long long để tránh tràn số)
pthread_mutex_t acc_mtx[70];     // Mỗi tài khoản có một mutex riêng biệt
Tx txs[5005];

int valid_cnt = 0;               // Biến đếm số giao dịch hợp lệ
pthread_mutex_t cnt_mtx = PTHREAD_MUTEX_INITIALIZER;

void *worker(void *arg) {
    int id = *(int *)arg;
    // Phân hoạch đều Q giao dịch cho T luồng
    int L = id * Q / T;
    int R = (id + 1) * Q / T;
    int local_cnt = 0;

    for (int i = L; i < R; i++) {
        int u = txs[i].u, v = txs[i].v, amt = txs[i].amount;

        // Bỏ qua các giao dịch không hợp lệ
        if (u < 0 || u >= M || v < 0 || v >= M || u == v || amt <= 0) continue;

        // Xác định thứ tự khóa tăng dần: min(u, v) trước, max(u, v) sau để chống Deadlock
        int first = (u < v) ? u : v;
        int second = (u < v) ? v : u;

        pthread_mutex_lock(&acc_mtx[first]);
        pthread_mutex_lock(&acc_mtx[second]);

        bal[u] -= amt;
        bal[v] += amt;

        pthread_mutex_unlock(&acc_mtx[second]);
        pthread_mutex_unlock(&acc_mtx[first]);

        local_cnt++;
    }

    // Cộng dồn số giao dịch hợp lệ của luồng vào biến chung có bảo vệ mutex
    pthread_mutex_lock(&cnt_mtx);
    valid_cnt += local_cnt;
    pthread_mutex_unlock(&cnt_mtx);
    return NULL;
}

int main() {
    if (scanf("%d %d %d", &M, &T, &Q) != 3) return 0;

    for (int i = 0; i < M; i++) {
        scanf("%lld", &bal[i]);
        pthread_mutex_init(&acc_mtx[i], NULL); // Khởi tạo mutex riêng cho tài khoản i
    }
    for (int i = 0; i < Q; i++) {
        scanf("%d %d %d", &txs[i].u, &txs[i].v, &txs[i].amount);
    }

    pthread_t tids[10];
    int ids[10];
    for (int i = 0; i < T; i++) {
        ids[i] = i;
        pthread_create(&tids[i], NULL, worker, &ids[i]);
    }

    for (int i = 0; i < T; i++) {
        pthread_join(tids[i], NULL);
    }

    // In dòng 1: số giao dịch hợp lệ; dòng 2: số dư cuối cùng của M tài khoản
    printf("%d\n", valid_cnt);
    for (int i = 0; i < M; i++) {
        printf("%lld%s", bal[i], (i + 1 < M) ? " " : "\n");
    }

    for (int i = 0; i < M; i++) pthread_mutex_destroy(&acc_mtx[i]);
    pthread_mutex_destroy(&cnt_mtx);
    return 0;
}