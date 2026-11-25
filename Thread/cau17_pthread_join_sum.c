#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int n, a[100005];

void *calc_sum(void *arg) {
    (void)arg;
    // Cấp phát động vùng nhớ trên Heap để không bị mất khi luồng con kết thúc
    long long *res = (long long *)malloc(sizeof(long long));
    *res = 0;
    for (int i = 0; i < n; i++) {
        *res += a[i]; // Cộng dồn các phần tử mảng vào vùng nhớ Heap
    }
    return res; // Trả địa chỉ vùng nhớ về cho luồng chính qua pthread_join()
}

int main() {
    if (scanf("%d", &n) != 1) return 0;
    for (int i = 0; i < n; i++) scanf("%d", &a[i]);

    pthread_t tid;
    pthread_create(&tid, NULL, calc_sum, NULL); // Khởi tạo luồng con tính tổng

    void *ret;
    pthread_join(tid, &ret); // Đợi luồng con hoàn tất và hứng con trỏ kết quả vào ret

    long long *ans = (long long *)ret;
    printf("%lld\n", *ans); // In giá trị tổng kiểu long long
    free(ans); // Giải phóng vùng nhớ đã cấp phát động trong luồng con
    return 0;
}