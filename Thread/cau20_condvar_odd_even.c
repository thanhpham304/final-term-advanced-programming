#include <stdio.h>
#include <pthread.h>

int n, cur = 1; // cur lưu số tiếp theo cần in (bắt đầu từ 1)
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *worker(void *arg) {
    int parity = *(int *)arg; // parity = 1: luồng in lẻ; parity = 0: luồng in chẵn
    for (int i = (parity == 1 ? 1 : 2); i <= n; i += 2) {
        pthread_mutex_lock(&mtx);
        while (cur % 2 != parity) { // Chưa đúng lượt chẵn/lẻ thì ngủ chờ
            pthread_cond_wait(&cond, &mtx);
        }
        printf("%d%s", cur, (cur < n) ? " " : "\n"); // Chỉ in dấu cách nếu chưa phải số cuối
        cur++;
        pthread_cond_signal(&cond); // Đánh thức luồng còn lại đến lượt
        pthread_mutex_unlock(&mtx);
    }
    return NULL;
}

int main() {
    if (scanf("%d", &n) != 1) return 0;

    pthread_t t1, t2;
    int odd = 1, even = 0;
    pthread_create(&t1, NULL, worker, &odd);  // Luồng 1 phụ trách số lẻ
    pthread_create(&t2, NULL, worker, &even); // Luồng 2 phụ trách số chẵn

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    pthread_mutex_destroy(&mtx);
    pthread_cond_destroy(&cond);
    return 0;
}