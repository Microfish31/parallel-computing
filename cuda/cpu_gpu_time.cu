#include <stdio.h>
#include <stdlib.h>
#include <cuda_runtime.h>
#include <time.h>


 int main(){
	
	
	struct timespec t_start, t_end;
	double elapsedTimeCPU;
	// start time
	clock_gettime( CLOCK_REALTIME, &t_start);
	
	//CPU computation
	
	// stop time
	clock_gettime( CLOCK_REALTIME, &t_end);
	// compute and print the elapsed time in millisec
	elapsedTimeCPU = (t_end.tv_sec - t_start.tv_sec) * 1000.0;
	elapsedTimeCPU += (t_end.tv_nsec - t_start.tv_nsec) / 1000000.0;
	printf("CPU elapsedTime: %lf ms\n", elapsedTimeCPU);
	
	
	
	
    // Get start time event
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start, 0);
	
	//GPU kernel function
	
	// Get stop time event    
    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop); 
    // Compute execution time
    float elapsedTime;
    cudaEventElapsedTime(&elapsedTime, start, stop);
    printf("GPU time: %13f msec\n", elapsedTime);
    cudaEventDestroy(start);
    cudaEventDestroy(stop);


	
	return 0;
 }