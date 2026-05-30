#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

static double GetTime(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + ts.tv_nsec * 1e-9;
}

static double f(double x) {
    return 4.0 / (1.0 + x * x);
}

double CalcPi(int n) {
    const double fH = 1.0 / (double)n;
    double fSum = 0.0;
    double fX;
    int i;

    for (i = 0; i < n; i++) {
        fX = fH * ((double)i + 0.5);
        fSum += f(fX);
    }
    return fH * fSum;
}

int main(int argc, char *argv[]) {
    int n = (argc > 1) ? atoi(argv[1]) : 100000000;

    double t0 = GetTime();
    double pi = CalcPi(n);
    double t1 = GetTime();

    printf("n       = %d\n", n);
    printf("pi      = %.15f\n", pi);
    printf("error   = %.2e\n", fabs(pi - M_PI));
    printf("time    = %.6f s\n", t1 - t0);

    return 0;
}
