#include <stdio.h>
#include <string.h>
#include <pthread.h>

char X[505], Y[505];
int memo[505][505]; // Bảng nhớ dùng chung lưu kết quả các bài toán con (-1: chưa tính)
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    int i, j, depth, res;
} Args;

int lcs(int i, int j, int depth);

void *worker(void *arg) {
    Args *a = (Args *)arg;
    a->res = lcs(a->i, a->j, a->depth);
    return NULL;
}

int lcs(int i, int j, int depth) {
    if (i == 0 || j == 0) return 0; // Bài toán cơ sở: một trong hai chuỗi rỗng

    pthread_mutex_lock(&mtx);
    int val = memo[i][j];
    pthread_mutex_unlock(&mtx); // Mở khóa ngay trước khi đệ quy hoặc tạo luồng để tránh deadlock
    if (val != -1) return val;

    if (X[i - 1] == Y[j - 1]) {
        val = 1 + lcs(i - 1, j - 1, depth);
    } else if (depth == 0) {
        // Tại nhánh rẽ đầu tiên, tạo 2 luồng tính song song LCS(i - 1, j) và LCS(i, j - 1)
        pthread_t t1, t2;
        Args a1 = {i - 1, j, 1, 0};
        Args a2 = {i, j - 1, 1, 0};

        pthread_create(&t1, NULL, worker, &a1);
        pthread_create(&t2, NULL, worker, &a2);

        pthread_join(t1, NULL); // Không giữ khóa mutex trong lúc chờ luồng con hoàn tất
        pthread_join(t2, NULL);

        val = (a1.res > a2.res) ? a1.res : a2.res;
    } else {
        int r1 = lcs(i - 1, j, depth);
        int r2 = lcs(i, j - 1, depth);
        val = (r1 > r2) ? r1 : r2;
    }

    pthread_mutex_lock(&mtx);
    memo[i][j] = val; // Lưu kết quả vào bảng nhớ dùng chung
    pthread_mutex_unlock(&mtx);

    return val;
}

int main() {
    if (scanf("%500s %500s", X, Y) != 2) return 0;

    int m = strlen(X);
    int n = strlen(Y);
    memset(memo, -1, sizeof(memo)); // Khởi tạo toàn bộ bảng nhớ bằng -1

    printf("%d\n", lcs(m, n, 0));

    pthread_mutex_destroy(&mtx);
    return 0;
}