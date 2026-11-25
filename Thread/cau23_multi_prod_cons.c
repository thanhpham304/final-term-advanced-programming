#include <stdio.h>
#include <pthread.h>

int P, C, B, n;
int a[10005];
int buf[32]; // Hàng đợi vòng dung lượng giới hạn B (1 <= B <= 32)
int head = 0, tail = 0, count = 0;
int active_producers; // Đếm số Producer chưa hoàn thành công việc
long long even_sum = 0;

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

void *producer(void *arg) {
    int id = *(int *)arg;
    // Phân hoạch đều đoạn chỉ số [L, R) cho Producer thứ id
    int L = id * n / P;
    int R = (id + 1) * n / P;

    for (int i = L; i < R; i++) {
        pthread_mutex_lock(&mtx);
        while (count == B) { // Hàng đợi đầy thì ngủ chờ Consumer rút bớt
            pthread_cond_wait(&not_full, &mtx);
        }
        buf[tail] = a[i];
        tail = (tail + 1) % B;
        count++;
        pthread_cond_signal(&not_empty); // Báo có dữ liệu mới cho Consumer
        pthread_mutex_unlock(&mtx);
    }

    // Báo hiệu Producer này đã đẩy xong phần việc của mình
    pthread_mutex_lock(&mtx);
    active_producers--;
    if (active_producers == 0) {
        pthread_cond_broadcast(&not_empty); // Producer cuối cùng đánh thức toàn bộ Consumer để thoát
    }
    pthread_mutex_unlock(&mtx);
    return NULL;
}

void *consumer(void *arg) {
    (void)arg;
    while (1) {
        pthread_mutex_lock(&mtx);
        // Chỉ ngủ chờ khi hàng đợi đang rỗng VÀ vẫn còn Producer đang chạy
        while (count == 0 && active_producers > 0) {
            pthread_cond_wait(&not_empty, &mtx);
        }
        // Hàng đợi đã rỗng và toàn bộ Producer đã xong -> thoát vòng lặp an toàn
        if (count == 0 && active_producers == 0) {
            pthread_mutex_unlock(&mtx);
            break;
        }

        int val = buf[head];
        head = (head + 1) % B;
        count--;
        if (val % 2 == 0) even_sum += val; // Kiểm tra chẵn đúng cho cả số âm và cộng dồn

        pthread_cond_signal(&not_full); // Báo hàng đợi đã trống thêm 1 chỗ cho Producer
        pthread_mutex_unlock(&mtx);
    }
    return NULL;
}

int main() {
    if (scanf("%d %d %d %d", &P, &C, &B, &n) != 4) return 0;
    for (int i = 0; i < n; i++) scanf("%d", &a[i]);

    active_producers = P;
    pthread_t prod_t[10], cons_t[10];
    int ids[10];

    for (int i = 0; i < P; i++) {
        ids[i] = i;
        pthread_create(&prod_t[i], NULL, producer, &ids[i]);
    }
    for (int i = 0; i < C; i++) {
        pthread_create(&cons_t[i], NULL, consumer, NULL);
    }

    for (int i = 0; i < P; i++) pthread_join(prod_t[i], NULL);
    for (int i = 0; i < C; i++) pthread_join(cons_t[i], NULL);

    printf("%lld\n", even_sum);
    pthread_mutex_destroy(&mtx);
    pthread_cond_destroy(&not_full);
    pthread_cond_destroy(&not_empty);
    return 0;
}