#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    long long a = 0;
    long long b = 0;

    if (scanf("%lld %lld", &a, &b) != 2 || a < 0 || a > b) {
        return 1;
    }

    fflush(stdout);
    pid_t pid = fork();

    if (pid < 0) {
        return 1;
    }

    if (pid == 0) {
        long long sum = 0;
        for (long long i = a; i <= b; i++) {
            sum += i;
        }
        int rem = (int)(sum % 200);
        _exit(rem);
    }

    int status = 0;
    if (waitpid(pid, &status, 0) < 0) {
        return 1;
    }

    if (WIFEXITED(status)) {
        printf("%d\n", WEXITSTATUS(status));
    }

    return 0;
}