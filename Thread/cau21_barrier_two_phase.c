#include <stdio.h>
#include <pthread.h>

int k, arrived = 0; // arrived đếm số luồng đã hoàn thành Pha 1 và chạm tới Barrier
long long a[20], b[20];
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *worker(void *arg) {
    int i = *(int *)arg; // Chỉ số riêng (0..k-1) của luồng

    // Pha 1: mỗi luồng tự bình phương phần tử a[i] của riêng mình
    a[i] = a[i] * a[i];

    // Barrier đồng bộ: chặn toàn bộ luồng cho tới khi đủ k luồng xong Pha 1
    pthread_mutex_lock(&mtx);
    arrived++;
    if (arrived == k) {
        pthread_cond_broadcast(&cond); // Luồng cuối cùng tới Barrier đánh thức tất cả các luồng đang chờ
    } else {
        while (arrived < k) {
            pthread_cond_wait(&cond, &mtx); // Chưa đủ k luồng thì nhả khóa và ngủ chờ
        }
    }
    pthread_mutex_unlock(&mtx);

    // Pha 2: đọc giá trị mới của a[i] và phần tử kế tiếp vòng tròn để tính b[i]
    b[i] = a[i] + a[(i + 1) % k];
    return NULL;
}

int main() {
    if (scanf("%d", &k) != 1) return 0;
    for (int i = 0; i < k; i++) scanf("%lld", &a[i]);

    pthread_t tids[20];
    int ids[20];
    for (int i = 0; i < k; i++) {
        ids[i] = i;
        pthread_create(&tids[i], NULL, worker, &ids[i]); // Tạo đúng k luồng
    }

    for (int i = 0; i < k; i++) {
        pthread_join(tids[i], NULL); // Chờ cả k luồng hoàn thành xong cả 2 pha
    }

    for (int i = 0; i < k; i++) {
        printf("%lld%s", b[i], (i + 1 < k) ? " " : "\n"); // In mảng b chuẩn định dạng, không dư dấu cách cuối
    }

    pthread_mutex_destroy(&mtx);
    pthread_cond_destroy(&cond);
    return 0;
}