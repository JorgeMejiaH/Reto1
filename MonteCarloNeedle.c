#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h> 
#include <sys/shm.h> 

double estimate_pi(double start_drops, double end_drops, double needle_length, double line_spacing) {
    long crosses = 0;

    for (long i = start_drops; i < end_drops; i++) {
        double x = (double)rand() / RAND_MAX;  // Posición del extremo de la aguja [0, 1]
        double angle = (double)rand() / RAND_MAX * M_PI;  // Ángulo entre la aguja y las líneas [0, π/2]

        if (x <= needle_length * 0.5 * cos(angle)) {
            crosses++;
        }
    }

    // Calcular la probabilidad de cruce
    double probability = (double)crosses / (end_drops-start_drops);

    // Estimar π
    double pi_estimate = ((2 * needle_length) / (probability * line_spacing)) / 2;
    return pi_estimate;
}

int main(int argc, char *argv[]) {
    srand((unsigned)time(NULL));  // Inicializar generador de números aleatorios

    struct timespec start, end;
    double elapsed_time;
    double total_drops = atoi(argv[1]);  // Número total de lanzamientos de la aguja
    double needle_length = 1.0;  // Longitud de la aguja
    double line_spacing = 2.0;   // Espaciado entre las líneas
    int verbose = 0;
    int num_processes = atoi(argv[2]); 
    double *shared_pi;

    if (argc > 3 && strcmp(argv[3], "-v") == 0) {//comando verbose
        verbose = 1;
    }

    clock_gettime(CLOCK_MONOTONIC, &start); //Inicia Captura del tiempo

    int shmid;
    shmid = shmget(IPC_PRIVATE, total_drops * sizeof(double), IPC_CREAT | 0666);
    if(shmid == -1 && verbose){
        perror("shmget");
        exit(1);
    }

    shared_pi = shmat(shmid, NULL, 0);

    // Calcular la cantidad de lanzamientos por proceso hijo
    double drops_per_process = total_drops / num_processes;

    // Crear num_processes hijos
    for (int i = 0; i < num_processes; i++) {
        pid_t pid = fork();

        if (pid == 0) { // Proceso hijo
            double start_drop = i * drops_per_process;
            double end_drop = (i == num_processes - 1) ? total_drops : (i + 1) * drops_per_process;
            double locla_pi = estimate_pi(start_drop, end_drop, needle_length, line_spacing);
            *shared_pi = locla_pi;
            shmdt(shared_pi);//Libera Memoria compartida
            exit(0);
        } else if (pid < 0) { // Error al crear el proceso hijo
            perror("Error en fork");
            exit(1);
        }
    }

    // Esperar a que todos los procesos hijos terminen
    int status;
    for (int i = 0; i < num_processes; i++) {
        wait(&status);
    }

    if(verbose){
            printf("%lf\n", *shared_pi);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf(" %f\n", elapsed_time);

    // Liberar recursos de memoria compartida
    shmdt(shared_pi);

    return 0;
}
