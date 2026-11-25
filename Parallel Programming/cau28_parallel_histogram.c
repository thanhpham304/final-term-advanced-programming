#include <stdio.h>
#include <pthread.h>

int n, T;
int a[1000005];
int all_hist[20][256]; // Lưu mảng tần suất cục bộ của từng luồng để luồng chính gộp lại

void *worker(void *arg) {
    int tid = *(int *)arg;
    int start = tid * n / T;     // Chỉ số bắt đầu đoạn mảng của luồng tid
    int end = (tid + 1) * n / T; // Chỉ số kết thúc đoạn mảng của luồng tid

    int local_hist[256] = {0};   // Mảng đếm tần suất cục bộ riêng của luồng (không cần mutex)
    for (int i = start; i < end; i++) {
        local_hist[a[i]]++;
    }

    // Lưu kết quả đếm cục bộ vào hàng riêng của luồng trong mảng dùng chung
    for (int v = 0; v < 256; v++) {
        all_hist[tid][v] = local_hist[v];
    }
    return NULL;
}

int main() {
    if (scanf("%d %d", &n, &T) != 2) return 0;
    for (int i = 0; i < n; i++) scanf("%d", &a[i]);

    pthread_t tids[20];
    int ids[20];
    for (int i = 0; i < T; i++) {
        ids[i] = i;
        pthread_create(&tids[i], NULL, worker, &ids[i]); // Pha Map: tạo T luồng đếm song song
    }

    int hist[256] = {0}; // Mảng histogram tổng hợp toàn cục
    for (int i = 0; i < T; i++) {
        pthread_join(tids[i], NULL); // Chờ luồng i hoàn tất
        // Pha Reduce: gộp tần suất cục bộ của luồng i vào hist chung
        for (int v = 0; v < 256; v++) {
            hist[v] += all_hist[i][v];
        }
    }

    int distinct = 0, mode_val = 0, mode_freq = 0;
    for (int v = 0; v < 256; v++) {
        if (hist[v] > 0) distinct++; // Đếm số giá trị pixel xuất hiện ít nhất 1 lần
        if (hist[v] > mode_freq) {   // Dùng dấu '>' nghiêm ngặt để giữ giá trị nhỏ nhất nếu hòa tần suất
            mode_freq = hist[v];
            mode_val = v;
        }
    }

    printf("%d %d %d\n", distinct, mode_val, mode_freq);
    return 0;
}