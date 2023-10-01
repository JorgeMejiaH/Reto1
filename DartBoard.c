#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h> 
#include <sys/shm.h> 

typedef struct {
    double pi_estimate;
} PiEstimateData;

double estimate_pi(double start_throw, double end_throw) {
    double crosses = 0;
    const double factor = 1.0 / RAND_MAX;

    for (double i = start_throw; i < end_throw; i++) {
        double x = rand() * factor;
        double y = rand() * factor;

        if (x * x + y * y < 1) {
            crosses++;
        }
    }

    double probability = (double)crosses / (end_throw-start_throw);
    double pi_estimate = 4.0 * probability;

    return pi_estimate;
}

int main(int argc, char *argv[]){
    struct timespec start, end;
    double elapsed_time;
    double total_throws;
    const double factor = 1.0 / RAND_MAX;
    total_throws = atoi(argv[1]);
    int verbose = 0;
    int num_processes = atoi(argv[2]);
    double *shared_pi;

    if (argc > 3 && strcmp(argv[3], "-v") == 0) {//comando verbose
        verbose = 1;
    }

    clock_gettime(CLOCK_MONOTONIC, &start); //Inicia Captura del tiempo

    int shmid;
    shmid = shmget(IPC_PRIVATE, total_throws * sizeof(double), IPC_CREAT | 0666);
    if(shmid == -1 && verbose){
        perror("shmget");
        exit(1);
    }

    shared_pi = shmat(shmid, NULL, 0);
    

    //initialize random generator
    srand((unsigned)time(NULL));

    // Calcular la cantidad de lanzamientos por proceso hijo
    double throws_per_process = total_throws / num_processes;

    // Crear num_processes hijos
    for (int i = 0; i < num_processes; i++) {
        pid_t pid = fork();

        if (pid == 0) { // Proceso hijo
            double start_throw = i * throws_per_process;
            double end_throw = (i == num_processes - 1) ? total_throws : (i + 1) * throws_per_process;
            double local_pi = estimate_pi(start_throw, end_throw);
            *shared_pi = local_pi;
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

    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    if(verbose){
        printf("Estimated Pi: %lf\n", *shared_pi);
    }

    printf(" %f\n", elapsed_time);

    shmdt(shared_pi);

    return 0;
}