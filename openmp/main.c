#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#define N 100

struct timespec diff(struct timespec, struct timespec);

int main() {
    struct timespec t_start, t_end, t_temp;
    double elapsedTime;
    int i, j, k;
    int a[N][N], b[N][N], c[N][N], cc[N][N];
    
    srand(time(NULL));
    
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            a[i][j] = rand();
            b[i][j] = rand();
            c[i][j] = 0;
            cc[i][j] = 0;
        }
    }

    // Sequential Execution
    clock_gettime(CLOCK_REALTIME, &t_start);  
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            for (k = 0; k < N; k++) {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
    clock_gettime(CLOCK_REALTIME, &t_end);
    t_temp = diff(t_start, t_end);
    elapsedTime = t_temp.tv_sec * 1000 + (double)t_temp.tv_nsec / 1000000.0;
    printf("Sequential elapsedTime: %lf ms\n", elapsedTime);

    omp_set_num_threads(10000);

    // Parallel Execution
    clock_gettime(CLOCK_REALTIME, &t_start);  

    // Declare sum outside the parallel region
    int sum = 0; 
    #pragma omp parallel for shared(a,b,cc) private(i, j, k) schedule(static) collapse(2) reduction(+:sum)
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            sum = 0;
            for (k = 0; k < N; k++) {
                sum += a[i][k] * b[k][j];
            }
            cc[i][j] = sum;
        }
    }
    
    clock_gettime(CLOCK_REALTIME, &t_end);
    t_temp = diff(t_start, t_end);
    elapsedTime = t_temp.tv_sec * 1000 + (double)t_temp.tv_nsec / 1000000.0;
    printf("Parallel elapsedTime: %lf ms\n", elapsedTime);

    // Verification
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            if (cc[i][j] != c[i][j]) {
                printf("Test failed!!!\n");
                return 1;
            }
        }
    }
    printf("Test pass!!!\n"); 
    return 0;
}

struct timespec diff(struct timespec start, struct timespec end) {
    struct timespec temp;
    if ((end.tv_nsec - start.tv_nsec) < 0) {
        temp.tv_sec = end.tv_sec - start.tv_sec - 1;
        temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec - start.tv_sec;
        temp.tv_nsec = end.tv_nsec - start.tv_nsec;
    }
    return temp;
}
