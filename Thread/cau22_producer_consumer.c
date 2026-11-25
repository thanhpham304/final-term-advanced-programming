#include <stdio.h>
#include <pthread.h>

int B, n;
int x[10005];    // Mảng lưu dãy n số đầu vào
int buf[64];     // Hàng đợi vòng dung lượng giới hạn B (1 <= B <= 64)
int head = 0, tail = 0, count = 0;
long long sum = 0;

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

void *producer(void *arg) {
    (void)arg;
    for (int i = 0; i < n; i++) {
        pthread_mutex_lock(&mtx);
        while (count == B) { // Hàng đợi đầy thì nhả khóa và chờ Consumer lấy bớt
            pthread_cond_wait(&not_full, &mtx);
        }
        buf[tail] = x[i];          // Đẩy phần tử vào cuối hàng đợi vòng
        tail = (tail + 1) % B;
        count++;
        pthread_cond_signal(&not_empty); // Đánh thức Consumer vì hàng đợi đã có phần tử
        pthread_mutex_unlock(&mtx);
    }
    return NULL;
}

void *consumer(void *arg) {
    (void)arg;
    for (int i = 1; i <= n; i++) {
        pthread_mutex_lock(&mtx);
        while (count == 0) { // Hàng đợi rỗng thì nhả khóa và chờ Producer đẩy thêm
            pthread_cond_wait(&not_empty, &mtx);
        }
        int val = buf[head];       // Lấy phần tử ở đầu hàng đợi theo đúng thứ tự FIFO
        head = (head + 1) % B;
        count--;
        pthread_cond_signal(&not_full); // Đánh thức Producer vì hàng đợi đã có chỗ trống
        pthread_mutex_unlock(&mtx);

        sum += 1LL * i * val;      // Tính tổng trọng số i * val_i (chỉ số i tính từ 1)
    }
    return NULL;
}

int main() {
    if (scanf("%d %d", &B, &n) != 2) return 0;
    for (int i = 0; i < n; i++) scanf("%d", &x[i]);

    pthread_t prod_t, cons_t;
    pthread_create(&prod_t, NULL, producer, NULL); // Khởi tạo luồng Producer
    pthread_create(&cons_t, NULL, consumer, NULL); // Khởi tạo luồng Consumer

    pthread_join(prod_t, NULL); // Chờ cả 2 luồng hoàn tất công việc
    pthread_join(cons_t, NULL);

    printf("%lld\n", sum);
    pthread_mutex_destroy(&mtx);
    pthread_cond_destroy(&not_full);
    pthread_cond_destroy(&not_empty);
    return 0;
}