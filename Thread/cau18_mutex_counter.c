#include <stdio.h>
#include <pthread.h>

long long counter = 0; // Biến đếm toàn cục dùng chung cho các luồng
int inc, dec;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER; // Khởi tạo tĩnh mutex bảo vệ counter

void *worker(void *arg) {
    int id = *(int *)arg; // Lấy chỉ số thứ tự (1..k) của luồng
    if (id % 2 == 1) {
        // Luồng chỉ số lẻ: tăng counter lên 1 đúng inc lần
        for (int i = 0; i < inc; i++) {
            pthread_mutex_lock(&mtx);   // Khóa vùng găng trước khi cập nhật counter
            counter++;
            pthread_mutex_unlock(&mtx); // Mở khóa để luồng khác truy cập
        }
    } else {
        // Luồng chỉ số chẵn: giảm counter đi 1 đúng dec lần
        for (int i = 0; i < dec; i++) {
            pthread_mutex_lock(&mtx);
            counter--;
            pthread_mutex_unlock(&mtx);
        }
    }
    return NULL;
}

int main() {
    int k;
    if (scanf("%d %d %d", &k, &inc, &dec) != 3) return 0;

    pthread_t tids[20];
    int ids[20]; // Mảng lưu chỉ số riêng cho từng luồng, tránh lỗi truyền chung địa chỉ &i

    for (int i = 1; i <= k; i++) {
        ids[i] = i;
        pthread_create(&tids[i], NULL, worker, &ids[i]); // Khởi tạo luồng thứ i
    }

    for (int i = 1; i <= k; i++) {
        pthread_join(tids[i], NULL); // Chờ toàn bộ k luồng chạy xong
    }

    pthread_mutex_destroy(&mtx); // Giải phóng tài nguyên mutex
    printf("%lld\n", counter);
    return 0;
}