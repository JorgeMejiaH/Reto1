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

    if (argc > 3 && strcmp(argv[3], "-v") == 0) {//comando verbose
        verbose = 1;
    }

    clock_gettime(CLOCK_MONOTONIC, &start); //Inicia Captura del tiempo

    int shm_fd = shm_open("/pi_estimate", O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }

    ftruncate(shm_fd, sizeof(PiEstimateData));
    PiEstimateData *pi_estimate_data = (PiEstimateData *)mmap(NULL, sizeof(PiEstimateData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (pi_estimate_data == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    pi_estimate_data->pi_estimate = 0.0;

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
            double pi_estimate = estimate_pi(start_throw, end_throw);
            pi_estimate_data->pi_estimate += pi_estimate;
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

    double total_pi_estimate = (pi_estimate_data->pi_estimate) / num_processes;

    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    if(verbose){
        printf("Pi aprox: %lf\n", total_pi_estimate);
    }

    printf(" %f\n", elapsed_time);

    // Liberar recursos de memoria compartida
    munmap(pi_estimate_data, sizeof(PiEstimateData));
    close(shm_fd);
    shm_unlink("/pi_estimate");

    return 0;
}