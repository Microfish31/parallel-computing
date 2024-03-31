#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 5

void *thread_function(void *);
 
int main() {
    pthread_t threads[NUM_THREADS];
    int thread_args[NUM_THREADS];
    int i, result;

    // Create threads
    for (i = 0; i < NUM_THREADS; i++) {
        thread_args[i] = i;
        result = pthread_create(&threads[i], NULL, thread_function, &thread_args[i]);
        if (result != 0) {
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        }
    }

     // Join threads
    for (i = 0; i < NUM_THREADS; i++) {
        result = pthread_join(threads[i], NULL);
        if (result != 0) {
            perror("Thread join failed");
            exit(EXIT_FAILURE);
        }
    }

    printf("Main: All threads have exited successfully.\n");

    return 0;
}

void *thread_function(void *arg) {
    int thread_id = *((int *)arg);
    printf("Thread %d: Hello, World!\n", thread_id);
    pthread_exit(NULL);
}
