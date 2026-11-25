#include <stdio.h>
#include <pthread.h>

int n, threshold, max_depth;
int a[100005];

// Cấu trúc truyền phạm vi đoạn [left, right] và độ sâu đệ quy hiện tại cho luồng
typedef struct {
    int left;
    int right;
    int depth;
} Range;

// Hàm phân hoạch Hoare: đưa các phần tử <= pivot sang trái, >= pivot sang phải
int partition(int left, int right) {
    int pivot = a[left + (right - left) / 2]; // Chọn phần tử giữa làm chốt để tránh suy biến O(n^2)
    int i = left - 1, j = right + 1;
    while (1) {
        do { i++; } while (a[i] < pivot);
        do { j--; } while (a[j] > pivot);
        if (i >= j) return j; // Trả về điểm chia j: nửa trái [left..j], nửa phải [j+1..right]
        int tmp = a[i];
        a[i] = a[j];
        a[j] = tmp;
    }
}

// Quicksort tuần tự dùng khi đoạn nhỏ hơn ngưỡng hoặc đã chạm giới hạn độ sâu
void seq_qsort(int left, int right) {
    if (left >= right) return;
    int p = partition(left, right);
    seq_qsort(left, p);
    seq_qsort(p + 1, right);
}

void *parallel_qsort(void *arg) {
    Range *r = (Range *)arg;
    int left = r->left, right = r->right, depth = r->depth;
    if (left >= right) return NULL;

    int p = partition(left, right);
    int len = right - left + 1;

    // Chỉ sinh thêm luồng mới khi độ dài đoạn > threshold VÀ chưa chạm max_depth
    if (len > threshold && depth < max_depth) {
        Range left_arg = {left, p, depth + 1};
        Range right_arg = {p + 1, right, depth + 1};

        pthread_t tid;
        pthread_create(&tid, NULL, parallel_qsort, &left_arg); // Sinh luồng con xử lý nhánh trái
        parallel_qsort(&right_arg);                            // Luồng hiện tại xử lý nhánh phải song song
        pthread_join(tid, NULL);                               // Đợi luồng nhánh trái hoàn tất
    } else {
        seq_qsort(left, p);      // Chuyển sang sắp xếp tuần tự nhánh trái
        seq_qsort(p + 1, right); // Chuyển sang sắp xếp tuần tự nhánh phải
    }
    return NULL;
}

int main() {
    if (scanf("%d %d %d", &n, &threshold, &max_depth) != 3) return 0;
    for (int i = 0; i < n; i++) scanf("%d", &a[i]);

    Range root = {0, n - 1, 0};
    parallel_qsort(&root); // Bắt đầu sắp xếp từ toàn mảng [0, n-1] ở độ sâu 0

    for (int i = 0; i < n; i++) {
        printf("%d%s", a[i], (i + 1 < n) ? " " : "\n"); // In mảng tăng dần, không dư dấu cách cuối
    }
    return 0;
}