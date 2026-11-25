#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    char buf[65540];
    // Cha đọc toàn bộ văn bản từ stdin tới EOF (tối đa 65536 byte)
    size_t len = fread(buf, 1, 65536, stdin);

    int in_p[2], out_p[2]; // in_p: cha -> con (stdin), out_p: con -> cha (stdout)
    pipe(in_p);
    pipe(out_p);

    pid_t pid = fork();
    if (pid == 0) {
        // Đóng các đầu ống không dùng ở tiến trình con
        close(in_p[1]);
        close(out_p[0]);

        // Chuyển hướng stdin và stdout của con sang hai đường ống
        dup2(in_p[0], STDIN_FILENO);
        dup2(out_p[1], STDOUT_FILENO);

        // Đóng các file descriptor gốc sau khi đã dup2
        close(in_p[0]);
        close(out_p[1]);

        // Thực thi lệnh wc -w thay thế tiến trình con
        execlp("wc", "wc", "-w", NULL);
        _exit(1);
    }

    // Tiến trình cha: đóng các đầu ống không dùng
    close(in_p[0]);
    close(out_p[1]);

    // Truyền toàn bộ văn bản sang ống đầu vào cho wc
    size_t sent = 0;
    while (sent < len) {
        ssize_t w = write(in_p[1], buf + sent, len - sent);
        if (w <= 0) break;
        sent += w;
    }
    close(in_p[1]); // Bắt buộc đóng đầu ghi để wc nhận biết EOF và bắt đầu đếm

    // Đọc kết quả từ ống đầu ra của wc
    char res[64] = {0};
    ssize_t r = read(out_p[0], res, sizeof(res) - 1);
    close(out_p[0]);

    waitpid(pid, NULL, 0); // Thu gom tiến trình con

    // Tách lấy đúng số nguyên (loại bỏ khoảng trắng/xuống dòng thừa) và in ra
    int words = 0;
    if (r > 0) sscanf(res, "%d", &words);
    printf("%d\n", words);

    return 0;
}