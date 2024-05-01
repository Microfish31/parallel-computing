#include <stdio.h>
#include <stdlib.h>
#include <cuda_runtime.h>

 __global__ void add(int *d_a, int *d_b, int *d_c){
	*d_c = *d_a + *d_b;
 }

 int main(){
	int a, b, c;
	int *d_a, *d_b, *d_c;

	cudaMalloc((void**)&d_a, sizeof(int));
	cudaMalloc((void**)&d_b, sizeof(int));
	cudaMalloc((void**)&d_c, sizeof(int));
	a = 7;
	b = 2;
	cudaMemcpy(d_a, &a, sizeof(int), cudaMemcpyHostToDevice);
	cudaMemcpy(d_b, &b, sizeof(int), cudaMemcpyHostToDevice);
	
	add<<<1, 1>>>(d_a, d_b, d_c);

	cudaMemcpy(&c, d_c, sizeof(int), cudaMemcpyDeviceToHost);
	printf("%d + %d = %d\n", a, b, c);
	
	//printf("%d + %d = %d\n", *d_a, *d_b, *d_c);//segmentation fault不可以直接對指到device memory的指標pointer做dereference

	return 0;
 }