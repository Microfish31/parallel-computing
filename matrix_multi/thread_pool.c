// #include <stdio.h>
// #include <stdlib.h>
// #include <pthread.h>

// #define THREAD_POOL_SIZE 12
// #define N 100

// int A [N][N];
// int B [N][N];

// typedef struct {
//     int start;
//     int end;
// } TaskInfo;

// typedef struct {
//     pthread_t threads[THREAD_POOL_SIZE];
//     int task_count;
//     TaskInfo tasks[THREAD_POOL_SIZE];
//     pthread_mutex_t lock;
//     pthread_cond_t condition;
//     int current_task;
//     int total_tasks;
// } ThreadPool;

// void *thread_pool_worker(ThreadPool *pool) {
//     while (1) {
//         pthread_mutex_lock(&pool->lock);

//         while (pool->task_count == 0 && pool->current_task < pool->total_tasks) {
//             pthread_cond_wait(&pool->condition, &pool->lock);
//         }

//         if (pool->current_task >= pool->total_tasks) {
//             pthread_mutex_unlock(&pool->lock);
//             break;
//         }

//         TaskInfo task = pool->tasks[--pool->task_count];
//         ++pool->current_task;
//         pthread_mutex_unlock(&pool->lock);

//         // Execute task
//         for (int i = task.start; i < task.end; ++i) {
//             // Run your task here
//             printf("Thread %ld processing task %d\n", pthread_self(), i);
//         }

       
//         struct v *data = param; 
//         int sum = 0; 
//         for(int k = 0; k<N; k++){
//             sum += A[data->i][k] * B[k][data->j];
//         }
//         C[data->i][data->j] = sum;
//         pthread_exit(0);


//     }

//     pthread_exit(NULL);
// }

// void thread_pool_init(ThreadPool *pool, int total_tasks) {
//     pool->task_count = 0;
//     pool->current_task = 0;
//     pool->total_tasks = total_tasks;
//     pthread_mutex_init(&pool->lock, NULL);
//     pthread_cond_init(&pool->condition, NULL);

//     for (int i = 0; i < THREAD_POOL_SIZE; ++i) {
//         pthread_create(&pool->threads[i], NULL, (void *(*)(void *))thread_pool_worker, pool);
//     }
// }

// void thread_pool_destroy(ThreadPool *pool) {
//     for (int i = 0; i < THREAD_POOL_SIZE; ++i) {
//         pthread_cancel(pool->threads[i]);
//         pthread_join(pool->threads[i], NULL);
//     }
//     pthread_mutex_destroy(&pool->lock);
//     pthread_cond_destroy(&pool->condition);
// }

// void thread_pool_add_task(ThreadPool *pool, int start, int end) {
//     pthread_mutex_lock(&pool->lock);

//     if (pool->task_count >= THREAD_POOL_SIZE) {
//         printf("Task queue is full.\n");
//         pthread_mutex_unlock(&pool->lock);
//         return;
//     }

//     TaskInfo *task = &pool->tasks[pool->task_count++];
//     task->start = start;
//     task->end = end;

//     pthread_cond_signal(&pool->condition);
//     pthread_mutex_unlock(&pool->lock);
// }



// int main() {
//     int i, j;
// 	// struct timespec t_start, t_end, t_temp ;
// 	// double elapsedTime;；
	
// 	// Data initialization
// 	for(i = 0; i < N; i++) {
// 	    for(j = 0; j < N; j++) {
// 			A[i][j] = rand()%100;
// 			B[i][j] = rand()%100;
// 		}
// 	}

//     int n = 12; // Adjust this value according to the number of tasks
//     int task_per_thread = n / THREAD_POOL_SIZE;
//     ThreadPool pool;
//     thread_pool_init(&pool, n);

//     // Add tasks to the thread pool
//     for(i = 0; i < N; i++) {
// 	    for(j = 0; j < N; j++) {
// 			thread_pool_add_task(&pool, i, i + task_per_thread);
// 		}
// 	}

//     // Wait for all tasks to complete
//     while (pool.current_task < n) {
//         // Wait
//     }

//     thread_pool_destroy(&pool);

//     return 0;
// }
