#include <stdio.h>
#include <pthread.h>

long long memo[100]; // Bảng nhớ dùng chung lưu các giá trị Fibonacci đã tính
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

// Hàm tính F(k) bằng Memoization có đồng bộ mutex an toàn
long long fib(int k) {
    pthread_mutex_lock(&mtx);
    long long val = memo[k];
    pthread_mutex_unlock(&mtx); // Mở khóa ngay trước khi đệ quy để tránh deadlock

    if (val != -1) return val;

    val = fib(k - 1) + fib(k - 2);

    pthread_mutex_lock(&mtx);
    memo[k] = val; // Ghi kết quả vào bảng nhớ dùng chung
    pthread_mutex_unlock(&mtx);

    return val;
}

void *worker(void *arg) {
    int k = *(int *)arg;
    fib(k);
    return NULL;
}

int main() {
    int n;
    if (scanf("%d", &n) != 1) return 0;

    for (int i = 0; i <= n; i++) memo[i] = -1;
    memo[0] = 0;
    memo[1] = 1;

    if (n < 2) {
        printf("%lld\n", memo[n]);
        return 0;
    }

    pthread_t t1, t2;
    int n1 = n - 1, n2 = n - 2;

    // Tạo 2 luồng tính song song F(n - 1) và F(n - 2) trên cùng bảng memo[]
    pthread_create(&t1, NULL, worker, &n1);
    pthread_create(&t2, NULL, worker, &n2);

    // Chờ cả 2 luồng hoàn tất (không giữ mutex khi gọi pthread_join)
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    memo[n] = memo[n - 1] + memo[n - 2];
    printf("%lld\n", memo[n]);

    pthread_mutex_destroy(&mtx);
    return 0;
}