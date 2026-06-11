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

Para el cálculo de π se utilizó el método de integración numérica basado en la regla del punto medio. La versión serial fue implementada en el archivo pi.c, donde un solo hilo ejecuta todo el ciclo encargado de calcular la suma de áreas bajo la curva. 

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

Compilación de la versión serial y Evaluación de Tiempo Serial

<img width="667" height="204" alt="Captura de pantalla 2026-06-11 081549" src="https://github.com/user-attachments/assets/50c2aaaa-3f11-41c4-a50e-6a1d2572a7cf" />

Se creó despues una copia de este programa llamada pi_p.c, en la cual se implementó una versión paralela usando Pthreads. Se divide entre varios hilos. Cada hilo recibe un rango de iteraciones y calcula una suma parcial. La creación de hilos se realizó con pthread_create y la sincronización se hizo con pthread_join. Una vez que todos los hilos terminaron, el hilo principal sumó los resultados parciales y calculó el valor final de π.

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

Compilación de la version paralela

<img width="736" height="241" alt="Captura de pantalla 2026-06-11 082425" src="https://github.com/user-attachments/assets/ded1b5f6-c8f3-4c01-b94f-1c3d99f46e85" />

Evaluación de Tiempo Paralelo con diferentes cantidades de hilos

<img width="648" height="600" alt="Captura de pantalla 2026-06-11 083306" src="https://github.com/user-attachments/assets/d7ceeed7-84c9-4016-acfc-65610e8d7ff2" />

Con los tiempos obtenidos se calcularon las métricas de rendimiento: Speedup y Eficiencia. Estas métricas permiten comparar el tiempo serial con los tiempos paralelos y analizar qué tan bien se aprovechan los hilos.

### 2 - Generador de Secuencia de Fibonacci

Se implementó el programa fibonacci.c, cuyo objetivo fue generar la secuencia de Fibonacci utilizando un hilo trabajador.

```c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

static double GetTime(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + ts.tv_nsec * 1e-9;
}

typedef struct {
    long long *arr;
    int        n;
} ThreadArgs;

static void *fib_worker(void *arg) {
    ThreadArgs *a = (ThreadArgs *)arg;
    long long  *arr = a->arr;
    int         n   = a->n;

    if (n >= 1) arr[0] = 0;
    if (n >= 2) arr[1] = 1;
    for (int i = 2; i < n; i++)
        arr[i] = arr[i - 1] + arr[i - 2];

    return NULL;
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

    ThreadArgs args = { arr, n };
    pthread_t  worker;

    double t0 = GetTime();
    pthread_create(&worker, NULL, fib_worker, &args);
    pthread_join(worker, NULL);
    double t1 = GetTime();

    if (n <= 20)
        for (int i = 0; i < n; i++)
            printf("F(%d) = %lld\n", i, arr[i]);

    printf("n    = %d\n", n);
    printf("time = %.9f s\n", t1 - t0);

    free(arr);
    return 0;
}
```

- El programa recibe por línea de comandos el número N de elementos que se desean generar. 
- El hilo principal reserva memoria dinámica para un arreglo de tamaño N usando malloc.
- El hilo principal crea un hilo trabajador mediante pthread_create.
- El hilo trabajador se encarga de calcular los valores de la secuencia de Fibonacci y almacenarlos directamente en el arreglo compartido.
- Una vez termina su ejecución, el hilo principal continúa después de llamar a pthread_join.
- El uso de pthread_join permite que el hilo principal espere a que el hilo trabajador termine antes de imprimir la secuencia.

Compilación y ejecución:

<img width="797" height="562" alt="Captura de pantalla 2026-06-11 092600" src="https://github.com/user-attachments/assets/5603237a-eb48-4aeb-ab2a-a14b2f628a4f" />

### 3 - Analisis

En este punto se desarrolló el notebook `analisis.ipynb`, donde se presentan los resultados obtenidos durante la ejecución de los programas `pi.c`, `pi_p.c` y `fibonacci.c`. Para el cálculo de π, se registró el tiempo de ejecución de la versión serial y se comparó con los tiempos obtenidos en la versión paralela usando diferentes cantidades de hilos. A partir de estos datos se calcularon las métricas de Speedup y Eficiencia, con el fin de analizar el impacto del paralelismo en el rendimiento.

Para la parte de Fibonacci, se mostró la salida del programa con 15 elementos y se explicó el diseño utilizado para compartir datos entre el hilo principal y el hilo trabajador. Se describió el papel de pthread_join como mecanismo de sincronización para asegurar que el hilo principal espere la finalización del cálculo antes de imprimir los resultados.

## Conclusiones

## Conclusiones

- Este laboratorio permitió comprender la importancia de los hilos en la programación concurrente y paralela. Se aprendió que una tarea grande puede dividirse en partes más pequeñas para ser ejecutada por varios hilos. Ya que esto ayudaria a mejorar el rendimiento cuando el problema se puede paralelizar correctamente.
- Se evidenció que el uso de varios hilos puede reducir el tiempo de ejecución en tareas computacionalmente pesadas; y que el paralelismo tiene costos asociados, como la creación y sincronización de hilos.
- Se implemento Fibonacci que permitió entender cómo compartir información entre hilos. Se utilizo la memoria dinámica para almacenar resultados y pasar datos mediante estructuras.
- El uso de pthread_join mostró la importancia de la sincronización en programas multihilo siendo importante ya que el hilo principal debe esperar la finalización de los hilos trabajadores antes de usar sus resultados. Esto evita errores, datos incompletos o comportamientos inesperados durante la ejecución.
- Usar más hilos no siempre significa obtener una mejora proporcional en el tiempo. El rendimiento depende del número de núcleos, la carga de trabajo y el diseño de la solución.
