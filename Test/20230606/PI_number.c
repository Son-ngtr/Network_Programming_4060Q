#include <stdio.h>
#include <time.h>

#define NUM_THREADS 8

static long num_steps = 1000000; 

int main() {

    int i;
    double x, sum = 0.0;
    double step = 1.0 / (double)num_steps;
    
    struct timespec start, stop;
    clock_gettime(CLOCK_REALTIME, &start);
    

    for (i = 0; i < num_steps; i++) 
    {
        x = (i + 0.5) * step;
        sum = sum + 4.0 / (1.0 + x * x);
    }
    clock_gettime(CLOCK_REALTIME, &stop);

    double time = (stop.tv_sec - start.tv_sec);
    double pi = step * sum;
    printf("PI = %.20f\n", pi);
    printf("%d \n", time);
}
