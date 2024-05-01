#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define M 100
#define K 100
#define N 100
#define NUM_THREADS 100

// Global var
int A[M][K];
int B[K][N];
int C[M][N];
int goldenC[M][N];

// Struct
struct v {
    int start;
    int end;
};

// Thread funcs
void *runner(void *param); 
struct timespec diff(struct timespec, struct timespec);

int main() {
    struct timespec t_start, t_end, t_temp;
    double elapsedTime;
    pthread_t tid[NUM_THREADS];       // Thread ID
    pthread_attr_t attr[NUM_THREADS]; // Set of thread attributes

    // Data initialization
    srand(time(NULL));

    for (int i = 0; i < M; i++) {
        for (int j = 0; j < K; j++) {
            A[i][j] = rand() % 100;
        }
    }

    for (int i = 0; i < K; i++) {
        for (int j = 0; j < N; j++) {
            B[i][j] = rand() % 100;
        }
    }

    // Parallel

    // Start time
    clock_gettime(CLOCK_REALTIME, &t_start);

    int gap = M / NUM_THREADS;

    for (int i = 0; i < NUM_THREADS; i++) {
        //Assign a row and column for each thread
        struct v *data = (struct v *)malloc(sizeof(struct v));
        data->start = i * gap;
        if (i == (NUM_THREADS-1) &&  data->end < M) {
            data->end = M;
        } else {
            data->end = (i + 1) * gap;
        }
        pthread_attr_init(&attr[i]);
        pthread_create(&tid[i], &attr[i], runner, data);
    }

    // Wait for threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(tid[i], NULL);
    }

    // Stop time
    clock_gettime(CLOCK_REALTIME, &t_end);

    // Compute and print the elapsed time in millisec
    t_temp = diff(t_start, t_end);
    elapsedTime = t_temp.tv_sec*1000 + (double)t_temp.tv_nsec / 1000000.0;
    printf("Parallel elapsedTime: %lf ms\n", elapsedTime);

    // ..............................................................

    // Sequential

    // Start time
    clock_gettime(CLOCK_REALTIME, &t_start);

    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < K; k++) {
                goldenC[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    // Stop time
    clock_gettime(CLOCK_REALTIME, &t_end);

    // Compute and print the elapsed time in millisec
    t_temp = diff(t_start, t_end);
    elapsedTime = t_temp.tv_sec*1000 + (double)t_temp.tv_nsec / 1000000.0;
    printf("Sequential elapsedTime: %lf ms\n", elapsedTime);

    // ..............................................................

    // Verification (parallel result equals sequential result)
    bool pass = true;

    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            if (goldenC[i][j] != C[i][j]) {
                printf("(%d,%d)\n", i, j);
                printf("%d\n", C[i][j]);
                printf("%d\n", goldenC[i][j]);
                pass = false;
                break;
            }
        }
        if (!pass) {
            break;
        }
    }

    if (pass)
        printf("Test pass!\n");
    else
        printf("Test failed!\n");

    // Success
    return 0;
}

void *runner(void *param) {
    struct v *data = (struct v *)param;
    int sum;
    // if (data->end != 0) {
    // printf("start: %d\n",data->start);
    // printf("end: %d\n",data->end);
    // }
    for (int i = data->start; i < data->end; i++) {
        for (int j = 0; j < N; j++) {
            sum = 0;
            for (int k = 0; k < K; k++) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }

    pthread_exit(0);
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
