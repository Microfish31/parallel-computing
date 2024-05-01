#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define M 100
#define K 100
#define N 100
#define NUM_THREADS 10000

int A [M][K];
int B [K][N];
int C [M][N];
int goldenC [M][N];
struct v {
   int i; /* row */
   int j; /* column */
};

void *runner(void *param); /* the thread */

int main() {
	int i, j, k;
	pthread_t tid[M][N];       //Thread ID
	pthread_attr_t attr[M][N]; //Set of thread attributes
	struct timespec t_start, t_end;
	double elapsedTime;
	
	for(i = 0; i < M; i++) {
	    for(j = 0; j < N; j++) {
			A[i][j] = rand()%100;
			B[i][j] = rand()%100;
		}
	}	
	
	// start time
	clock_gettime( CLOCK_REALTIME, &t_start);  	
	
	for(i = 0; i < M; i++) {
	    for(j = 0; j < N; j++) {
		    //Assign a row and column for each thread
			struct v *data = (struct v *) malloc(sizeof(struct v));
			data->i = i;
			data->j = j;
			/* Now create the thread passing it data as a parameter */
			//pthread_t tid;       //Thread ID
			//pthread_attr_t attr; //Set of thread attributes
			//Get the default attributes
			pthread_attr_init(&attr[i][j]);
			//Create the thread
			pthread_create(&tid[i][j],&attr[i][j],runner,data);
			//Make sure the parent waits for all thread to complete
			//pthread_join(tid, NULL);

		}
	}
	for(i = 0; i < M; i++) {
	    for(j = 0; j < N; j++) {
			pthread_join(tid[i][j], NULL);
		}
	}	
	// stop time
	clock_gettime( CLOCK_REALTIME, &t_end);

	// compute and print the elapsed time in millisec
	elapsedTime = (t_end.tv_sec - t_start.tv_sec) * 1000.0;
	elapsedTime += (t_end.tv_nsec - t_start.tv_nsec) / 1000000.0;
	printf("Parallel elapsedTime: %lf ms\n", elapsedTime);	
	//Print out the resulting matrix
	
	
	// start time
	clock_gettime( CLOCK_REALTIME, &t_start);  
	for(i = 0; i < M; i++) {
		for(j = 0; j < N; j++) {
			for(k=0; k<K; k++){
				goldenC[i][j]+=A[i][k] * B[k][j];
			}
		}
	}
	// stop time
	clock_gettime( CLOCK_REALTIME, &t_end);

	// compute and print the elapsed time in millisec
	elapsedTime = (t_end.tv_sec - t_start.tv_sec) * 1000.0;
	elapsedTime += (t_end.tv_nsec - t_start.tv_nsec) / 1000000.0;
	printf("Sequential elapsedTime: %lf ms\n", elapsedTime);	
	
	int pass = 1;
	for(i = 0; i < M; i++) {
		for(j = 0; j < N; j++) {
			if(goldenC[i][j]!=C[i][j]){
				pass = 0;
			}
		}
	}	
	if(pass==1)
		printf("Test pass!\n");
	
	return 0;
}

																					//The thread will begin control in this function
void *runner(void *param) {
	struct v *data = param; 
	int n, sum = 0; 
	for(n = 0; n< K; n++){
		sum += A[data->i][n] * B[n][data->j];
	}
	C[data->i][data->j] = sum;
	pthread_exit(0);
}