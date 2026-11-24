#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define MOD 1000000007LL

int main(void) {
    char file_path[256];
    int key = 0;

    if (scanf("%255s %d", file_path, &key) != 2) {
        printf("-1\n");
        return 0;
    }

    if (key < 0 || key > 255) {
        printf("-1\n");
        return 0;
    }

    int fd = open(file_path, O_RDWR);
    if (fd < 0) {
        printf("-1\n");
        return 0;
    }

    struct stat st;
    if (fstat(fd, &st) < 0 || st.st_size < 0) {
        close(fd);
        printf("-1\n");
        return 0;
    }

    if (st.st_size == 0) {
        close(fd);
        printf("0\n");
        return 0;
    }

    size_t size = (size_t)st.st_size;
    unsigned char *mapped = (unsigned char *)mmap(
        NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0
    );
    if (mapped == MAP_FAILED) {
        close(fd);
        printf("-1\n");
        return 0;
    }

    unsigned char xor_key = (unsigned char)key;
    long long total_sum = 0;

    for (size_t i = 0; i < size; i++) {
        mapped[i] ^= xor_key;
        total_sum += mapped[i];
    }

    munmap(mapped, size);
    close(fd);

    printf("%lld\n", total_sum % MOD);
    return 0;
}