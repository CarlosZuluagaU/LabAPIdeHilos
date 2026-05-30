#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static double GetTime(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + ts.tv_nsec * 1e-9;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <N>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    if (n <= 0) {
        fprintf(stderr, "Error: N debe ser mayor que 0\n");
        return 1;
    }

    long long *arr = malloc(n * sizeof(long long));
    if (!arr) {
        fprintf(stderr, "Error: malloc fallo\n");
        return 1;
    }

    double t0 = GetTime();
    if (n >= 1) arr[0] = 0;
    if (n >= 2) arr[1] = 1;
    for (int i = 2; i < n; i++)
        arr[i] = arr[i - 1] + arr[i - 2];
    double t1 = GetTime();

    if (n <= 20)
        for (int i = 0; i < n; i++)
            printf("F(%d) = %lld\n", i, arr[i]);

    printf("n    = %d\n", n);
    printf("time = %.9f s\n", t1 - t0);

    free(arr);
    return 0;
}
