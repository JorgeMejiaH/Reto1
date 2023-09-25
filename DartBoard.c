#include <stdlib.h>
#include <time.h>
#include <math.h>

int main(int argc, char *argv[]){
    long k, total_throws, hits;
    const double factor = 1.0 / RAND_MAX;
    cout("Enter the number of thoses");
    total_throws = atoi(argv[1]);
    int verbose = 0;

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
    printf("Pi aprox: %lf\n", Pi_aprox);
    return 0;
}