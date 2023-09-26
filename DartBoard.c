#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>

int main(int argc, char *argv[]){
    struct timespec start, end;
    double elapsed_time;
    long k, total_throws, hits;
    const double factor = 1.0 / RAND_MAX;
    total_throws = atoi(argv[1]);
    int verbose = 0;

    if (argc > 2 && strcmp(argv[2], "-v") == 0) {//comando verbose
        verbose = 1;
    }

    clock_gettime(CLOCK_MONOTONIC, &start); //Inicia Captura del tiempo

    //initialize random generator
    srand((unsigned)time(NULL));

    //Throw thosses
    for(k=hits=0; k<total_throws; k++){
        //Find two random numbers whithin 0..1
        double x_axis = rand() * factor;
        double y_axis = rand() * factor;
        if(x_axis*x_axis + y_axis*y_axis < 1){ //Inside de Circle
            hits++;
        }
    }
    double Pi_aprox = 4.0 * hits / total_throws;

     clock_gettime(CLOCK_MONOTONIC, &end);
     elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    if(verbose){
        printf("Pi aprox: %lf\n", Pi_aprox);
    }

    printf(" %f\n", elapsed_time);

    return 0;
}