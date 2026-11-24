#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

int main(void) {
    char dir_path[512];
    char ext[64];

    if (scanf("%511s %63s", dir_path, ext) != 2) {
        printf("-1\n");
        return 0;
    }

    DIR *dir = opendir(dir_path);
    if (dir == NULL) {
        printf("-1\n");
        return 0;
    }

    size_t dir_len = strlen(dir_path);
    size_t ext_len = strlen(ext);
    int has_trailing_slash = (dir_len > 0 && dir_path[dir_len - 1] == '/');

    long long matched_count = 0;
    long long total_size = 0;
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        size_t name_len = strlen(entry->d_name);
        if (name_len < ext_len || strcmp(entry->d_name + name_len - ext_len, ext) != 0) {
            continue;
        }

        char full_path[1024];
        if (has_trailing_slash) {
            snprintf(full_path, sizeof(full_path), "%s%s", dir_path, entry->d_name);
        } else {
            snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);
        }

        struct stat st;
        if (stat(full_path, &st) == 0 && S_ISREG(st.st_mode)) {
            matched_count++;
            total_size += (long long)st.st_size;
        }
    }

    closedir(dir);
    printf("%lld %lld\n", matched_count, total_size);
    return 0;
}