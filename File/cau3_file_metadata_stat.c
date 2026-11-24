#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

int main(void) {
    char path[512];

    if (scanf("%511s", path) != 1) {
        printf("-1\n");
        return 0;
    }

    struct stat st;
    if (stat(path, &st) < 0) {
        printf("-1\n");
        return 0;
    }

    const char *file_type;
    if (S_ISREG(st.st_mode)) {
        file_type = "REG";
    } else if (S_ISDIR(st.st_mode)) {
        file_type = "DIR";
    } else {
        file_type = "OTHER";
    }

    long long file_size = (long long)st.st_size;
    unsigned int perms = (unsigned int)(st.st_mode & 0777);

    printf("%s %lld %03o\n", file_type, file_size, perms);
    return 0;
}