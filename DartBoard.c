#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <pthread.h>

typedef struct { //Estructura para definir los valores de cada hilo
    double hits;
    double total_throws;
} ThreadResult;

// Función que realiza el lanzamiento de dardos y cuenta los aciertos
void *throw_darts(void *arg) {
    ThreadResult *result = (ThreadResult *)arg;
    const double factor = 1.0 / RAND_MAX;
    const double total_throws = result->total_throws;
    double hits = 0;

    for (double k = 0; k < total_throws; k++) {
        double x_axis = rand() * factor;
        double y_axis = rand() * factor;
        if (x_axis * x_axis + y_axis * y_axis < 1) {
            hits++;
        }
    }

    result->hits = hits;
    pthread_exit(NULL);
}

int main(int argc, char *argv[]){
    struct timespec start, end;
    double elapsed_time;
    double total_throws;
    const double factor = 1.0 / RAND_MAX;
    total_throws = atoi(argv[1]);
    int Num_Threads = atoi(argv[2]);
    int verbose = 0;

    if (argc > 3 && strcmp(argv[3], "-v") == 0) {//comando verbose
        verbose = 1;
    }

    clock_gettime(CLOCK_MONOTONIC, &start); //Inicia Captura del tiempo

    pthread_t threads[Num_Threads];
    ThreadResult thread_results[Num_Threads];

    //initialize random generator
    srand((unsigned)time(NULL));

    // Crear hilos y lanzar dardos
    for (int i = 0; i < Num_Threads; i++) {
        thread_results[i].total_throws = total_throws / Num_Threads;

        pthread_create(&threads[i], NULL, throw_darts, (void *)&thread_results[i]);
    }

    double total_hits = 0;
    // Esperar a que los hilos terminen
    for (int i = 0; i < Num_Threads; i++) {
        pthread_join(threads[i], NULL);
        total_hits += thread_results[i].hits;
    }

    double Pi_aprox = 4.0 * total_hits / total_throws;

    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    if(verbose){
        printf("Pi aprox: %lf\n", Pi_aprox);
    }

    printf(" %f\n", elapsed_time);

    return 0;
}