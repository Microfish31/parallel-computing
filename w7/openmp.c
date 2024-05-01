#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 100

int A [N][N];
int B [N][N];
int C [N][N];
int goldenC [N][N];

// transpose
// most big N?

int main() {
    int i, j, k;
    int sum;
    struct timespec t_start, t_end;
    double elapsedTime;
   
    for(i = 0; i < N; i++) {
        for(j = 0; j < N; j++) {
            A[i][j] = rand()%100;
            B[i][j] = rand()%100;
        }
    }  
     
     omp_set_num_threads(3);
    // openmp
    // start time
    clock_gettime( CLOCK_REALTIME, &t_start);  
    #pragma omp parallel for private(j,k,sum)
    for(i = 0; i < N; i++) {
        for(j = 0; j < N; j++) {
            sum = 0;
            #pragma omp parallel for reduction(+:sum)
            for(k=0; k<N; k++) {
               sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
    // stop time
    clock_gettime( CLOCK_REALTIME, &t_end);

    // compute and print the elapsed time in millisec
    elapsedTime = (t_end.tv_sec - t_start.tv_sec) * 1000.0;
    elapsedTime += (t_end.tv_nsec - t_start.tv_nsec) / 1000000.0;
    printf("Parallel elapsedTime: %lf ms\n", elapsedTime);  
    
    // Print out the resulting matrix

    // start time
    clock_gettime( CLOCK_REALTIME, &t_start);  
    for(i = 0; i < N; i++) {
        for(j = 0; j < N; j++) {
            sum = 0;
            for(k=0; k<N; k++){
                sum += A[i][k] * B[k][j];
            }
            goldenC[i][j] = sum;
        }
    }
        // stop time
    clock_gettime( CLOCK_REALTIME, &t_end);

    // compute and print the elapsed time in millisec
    elapsedTime = (t_end.tv_sec - t_start.tv_sec) * 1000.0;
    elapsedTime += (t_end.tv_nsec - t_start.tv_nsec) / 1000000.0;
    printf("Sequential elapsedTime: %lf ms\n", elapsedTime);    
   
    // Verification
    int pass = 1;
    for(i = 0; i < N; i++) {
        for(j = 0; j < N; j++) {
            if(goldenC[i][j]!=C[i][j]){
                pass = 0;
            }
        }
    }  
    if(pass==1)
        printf("Test pass!\n");
    else 
        printf("Test failed!\n");
   
    return 0;
}