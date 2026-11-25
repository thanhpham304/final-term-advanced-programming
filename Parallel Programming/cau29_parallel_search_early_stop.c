#include <stdio.h>
#include <pthread.h>

int n, target, T;
int a[100005];
int found = 0, found_idx = -1; // Cờ báo đã tìm thấy và vị trí 0-based tương ứng
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

void *worker(void *arg) {
    int tid = *(int *)arg;
    int start = tid * n / T;     // Chỉ số bắt đầu đoạn mảng của luồng tid
    int end = (tid + 1) * n / T; // Chỉ số kết thúc đoạn mảng của luồng tid

    for (int i = start; i < end; i++) {
        // Định kỳ kiểm tra cờ found qua mutex để dừng sớm nếu luồng khác đã tìm thấy
        if ((i - start) % 64 == 0) {
            pthread_mutex_lock(&mtx);
            int stop = found;
            pthread_mutex_unlock(&mtx);
            if (stop) break;
        }

        if (a[i] == target) {
            pthread_mutex_lock(&mtx);
            found = 1;
            found_idx = i; // Ghi nhận vị trí duy nhất tìm thấy target rồi dừng luồng
            pthread_mutex_unlock(&mtx);
            break;
        }
    }
    return NULL;
}

int main() {
    if (scanf("%d %d %d", &n, &target, &T) != 3) return 0;
    for (int i = 0; i < n; i++) scanf("%d", &a[i]);

    pthread_t tids[20];
    int ids[20]; // Mảng lưu chỉ số riêng (0..T-1) cho từng luồng
    for (int i = 0; i < T; i++) {
        ids[i] = i;
        pthread_create(&tids[i], NULL, worker, &ids[i]);
    }

    for (int i = 0; i < T; i++) {
        pthread_join(tids[i], NULL); // Chờ tất cả T luồng kết thúc
    }

    printf("%d\n", found_idx);
    pthread_mutex_destroy(&mtx);
    return 0;
}