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

typedef struct {
    double pi_estimate;
} PiEstimateData;

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

    // Calcular la cantidad de lanzamientos por proceso hijo
    double drops_per_process = total_drops / num_processes;

    // Crear num_processes hijos
    for (int i = 0; i < num_processes; i++) {
        pid_t pid = fork();

        if (pid == 0) { // Proceso hijo
            double start_drop = i * drops_per_process;
            double end_drop = (i == num_processes - 1) ? total_drops : (i + 1) * drops_per_process;
            double pi_estimate = estimate_pi(start_drop, end_drop, needle_length, line_spacing);
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

    if(verbose){
            printf("%lf\n", total_pi_estimate);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf(" %f\n", elapsed_time);

    // Liberar recursos de memoria compartida
    munmap(pi_estimate_data, sizeof(PiEstimateData));
    close(shm_fd);
    shm_unlink("/pi_estimate");

    return 0;
}
