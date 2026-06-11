# Laboratorio 04 (API de Hilos) - Sistemas Operativos 
Elaborado por: 
- Carlos Andres Zuluaga Amaya
  andres.zuluaga6@udea.edu.co
- Duván Antonio Arboleda Botero
  duvan.arboleda1@udea.edu.co

Link de video:

## Introducción
El propósito de este laboratorio es aplicar los conceptos de creación, ejecución y sincronización de hilos para resolver problemas computacionales 
usando programación multihilo. Se desarrollo dos ejercicios principales: la paralelización del cálculo de π mediante integración numérica y la 
generación de la secuencia de Fibonacci usando un hilo trabajador.

## Objetivo
Implementar programas en lenguaje C utilizando la API de hilos POSIX para comprender el funcionamiento de la programación multihilo, la sincronización entre 
hilos y el análisis de rendimiento en aplicaciones paralelas.

## Desarrollo

### 1 - Paralelización del Cálculo de π

Para el cálculo de π se utilizó el método de integración numérica basado en la regla del punto medio. La versión serial fue implementada en el archivo pi.c. 

```c
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
```

Luego vamos a compilar pi.c



Se creó una copia de este programa llamada pi_p.c, en la cual se implementó una versión paralela usando Pthreads.

```c
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

```

La estrategia utilizada fue paralelismo de datos, ya que todos los hilos ejecutan la misma operación, pero sobre diferentes partes del rango total de iteraciones.


### 2 - Generador de Secuencia de Fibonacci

