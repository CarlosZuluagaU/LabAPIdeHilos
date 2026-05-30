#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <pthread.h>

typedef struct {
    int    start;
    int    end;
    int    n;
    double partial_sum;
} ThreadArgs;

static double GetTime(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + ts.tv_nsec * 1e-9;
}

static void *calc_partial(void *arg) {
    ThreadArgs *a = (ThreadArgs *)arg;
    const double fH = 1.0 / (double)a->n;
    double sum = 0.0;
    int i;

    for (i = a->start; i < a->end; i++) {
        double x = fH * ((double)i + 0.5);
        sum += 4.0 / (1.0 + x * x);
    }

    a->partial_sum = sum;
    return NULL;
}

double CalcPi(int n, int T) {
    pthread_t  *threads = malloc(T * sizeof(pthread_t));
    ThreadArgs *args    = malloc(T * sizeof(ThreadArgs));
    int chunk = n / T;

    for (int t = 0; t < T; t++) {
        args[t].start = t * chunk;
        args[t].end   = (t == T - 1) ? n : (t + 1) * chunk;
        args[t].n     = n;
        pthread_create(&threads[t], NULL, calc_partial, &args[t]);
    }

    double total = 0.0;
    for (int t = 0; t < T; t++) {
        pthread_join(threads[t], NULL);
        total += args[t].partial_sum;
    }

    free(threads);
    free(args);

    return total / (double)n;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <n> <threads>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    int T = atoi(argv[2]);

    double t0 = GetTime();
    double pi = CalcPi(n, T);
    double t1 = GetTime();

    printf("n       = %d\n", n);
    printf("threads = %d\n", T);
    printf("pi      = %.15f\n", pi);
    printf("error   = %.2e\n", fabs(pi - M_PI));
    printf("time    = %.6f s\n", t1 - t0);

    return 0;
}
