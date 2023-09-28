#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <pthread.h>

typedef struct {
    double total_drops_per_thread;
    double needle_length;
    double line_spacing;
    double crosses;
} ThreadData;

void *thread_estimate_pi(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    data->crosses = 0;

    for (double i = 0; i < data->total_drops_per_thread; i++) {
        double x = (double)rand() / RAND_MAX;  
        double angle = (double)rand() / RAND_MAX * M_PI;  

        if (x <= data->needle_length * 0.5 * cos(angle)) {
            data->crosses++;
        }
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    srand((unsigned)time(NULL));  // Inicializar generador de números aleatorios

    struct timespec start, end;
    double elapsed_time;
    double total_drops = atoi(argv[1]);  // Número total de lanzamientos de la aguja
    double needle_length = 1.0;  // Longitud de la aguja
    double line_spacing = 2.0;   // Espaciado entre las líneas
    int verbose = 0;
    int Num_Thread = atoi(argv[2]);

    if (argc > 3 && strcmp(argv[3], "-v") == 0) {//comando verbose
        verbose = 1;
    }

    clock_gettime(CLOCK_MONOTONIC, &start); //Inicia Captura del tiempo

    pthread_t threads[Num_Thread];
    ThreadData thread_data[Num_Thread];

    double total_drops_per_thread = total_drops / Num_Thread;

    for (int i = 0; i < Num_Thread; i++) {
        thread_data[i].total_drops_per_thread = total_drops_per_thread;
        thread_data[i].needle_length = needle_length;
        thread_data[i].line_spacing = line_spacing;

        pthread_create(&threads[i], NULL, thread_estimate_pi, (void *)&thread_data[i]);
    }

    double total_crosses = 0;

    for (int i = 0; i < Num_Thread; i++) {
        pthread_join(threads[i], NULL);
        total_crosses += thread_data[i].crosses;
    }

    double pi_estimate = ((2 * needle_length) / ((total_crosses / total_drops) * line_spacing)) / 2;

    if(verbose){
            printf("%lf\n", pi_estimate);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf(" %f\n", elapsed_time);

    return 0;
}
