#include <stdio.h>
#include <pthread.h>

int a[100005];

// Cấu trúc lưu phạm vi [left, right] và kết quả cực đại cục bộ của từng luồng
typedef struct {
    int left;
    int right;
    int local_max;
} Task;

void *find_max(void *arg) {
    Task *t = (Task *)arg; // Ép kiểu về cấu trúc Task riêng của luồng này
    int mx = a[t->left];   // Khởi tạo bằng phần tử đầu đoạn để xử lý đúng cả mảng toàn số âm
    for (int i = t->left + 1; i <= t->right; i++) {
        if (a[i] > mx) mx = a[i];
    }
    t->local_max = mx;     // Ghi kết quả vào trường riêng của struct (không cần mutex)
    return NULL;
}

int main() {
    int n, k;
    if (scanf("%d %d", &n, &k) != 2) return 0;
    for (int i = 0; i < n; i++) scanf("%d", &a[i]);

    int m = (n < k) ? n : k; // Tạo tối đa min(n, k) luồng để không sinh luồng rỗng khi k > n
    pthread_t tids[20];
    Task tasks[20];          // Mảng struct riêng biệt cho từng luồng (1 <= k <= 16)

    for (int i = 0; i < m; i++) {
        // Chia đều đoạn chỉ số [left, right] kể cả khi n không chia hết cho m
        tasks[i].left = i * n / m;
        tasks[i].right = (i + 1) * n / m - 1;
        pthread_create(&tids[i], NULL, find_max, &tasks[i]); // Truyền địa chỉ phần tử struct riêng
    }

    for (int i = 0; i < m; i++) {
        pthread_join(tids[i], NULL); // Chờ tất cả m luồng tìm xong cực đại cục bộ
    }

    // Tổng hợp giá trị lớn nhất toàn mảng từ các local_max
    int ans = tasks[0].local_max;
    for (int i = 1; i < m; i++) {
        if (tasks[i].local_max > ans) ans = tasks[i].local_max;
    }

    printf("%d\n", ans);
    return 0;
}