#include <stdio.h>

int main( void ) {
    cudaDeviceProp  prop;
    FILE* fptr;
    fptr=fopen("GPUspec.txt","w");
    if(fptr==NULL){
       printf("FILE cannot be opened.\n");
       exit (1);
    }

    int count;
    cudaGetDeviceCount( &count );

    for (int i=0; i< count; i++) {
        cudaGetDeviceProperties( &prop, i );
        printf( "--- General Information for device %d ---\n", i );
        fprintf(fptr, "--- General Information for device %d ---\n", i );
        printf( "Name:  %s\n", prop.name );
        fprintf(fptr, "Name:  %s\n", prop.name );
        printf( "Compute capability:  %d.%d\n", prop.major, prop.minor );
        fprintf(fptr, "Compute capability:  %d.%d\n", prop.major, prop.minor );
        printf( "Clock rate:  %d\n", prop.clockRate );
        fprintf( fptr,"Clock rate:  %d\n", prop.clockRate );
        printf( "Device copy overlap:  " );
        fprintf( fptr,"Device copy overlap:  " );
        if (prop.deviceOverlap)
            printf( "Enabled\n" );
        else
            printf( "Disabled\n");

        printf( "Kernel execution timeout :  " );
        if (prop.kernelExecTimeoutEnabled)
            printf( "Enabled\n" );
        else
            printf( "Disabled\n" );

        printf("\n--- Memory Information for device %d ---\n", i );
        fprintf(fptr, "   --- Memory Information for device %d ---\n", i );
        printf("Total global mem:  %ld\n", prop.totalGlobalMem );
        fprintf(fptr, "Total global mem:  %ld\n", prop.totalGlobalMem );
        printf("Total constant Mem:  %ld\n", prop.totalConstMem );
        fprintf(fptr, "Total constant Mem:  %ld\n", prop.totalConstMem );
        printf("Max mem pitch:  %ld\n", prop.memPitch );
        fprintf(fptr, "Max mem pitch:  %ld\n", prop.memPitch );
        printf("Texture Alignment:  %ld\n", prop.textureAlignment );
        fprintf(fptr, "Texture Alignment:  %ld\n", prop.textureAlignment );
        printf("\n--- MP Information for device %d ---\n", i );
        fprintf(fptr, "--- MP Information for device %d ---\n", i );
        printf("Multiprocessor count:  %d\n",prop.multiProcessorCount );
        fprintf(fptr, "Multiprocessor count:  %d\n",prop.multiProcessorCount );
        printf("Shared mem per mp:  %ld\n", prop.sharedMemPerBlock );
        fprintf(fptr, "Shared mem per mp:  %ld\n", prop.sharedMemPerBlock );
        printf("Registers per mp:  %d\n", prop.regsPerBlock );
        fprintf(fptr, "Registers per mp:  %d\n", prop.regsPerBlock );
        printf("Threads in warp:  %d\n", prop.warpSize );
        fprintf(fptr, "Threads in warp:  %d\n", prop.warpSize );
        printf("Max threads per block:  %d\n",prop.maxThreadsPerBlock );
        fprintf(fptr, "Max threads per block:  %d\n",prop.maxThreadsPerBlock );
        printf("Max thread dimensions:  (%d, %d, %d)\n",prop.maxThreadsDim[0], prop.maxThreadsDim[1], prop.maxThreadsDim[2] );
        fprintf(fptr, "Max thread dimensions:  (%d, %d, %d)\n",prop.maxThreadsDim[0], prop.maxThreadsDim[1], prop.maxThreadsDim[2] );
        printf("Max grid dimensions:  (%d, %d, %d)\n",prop.maxGridSize[0], prop.maxGridSize[1],prop.maxGridSize[2] );
        fprintf(fptr, "Max grid dimensions:  (%d, %d, %d)\n",prop.maxGridSize[0], prop.maxGridSize[1],prop.maxGridSize[2] );
        printf("\n" );
    }
    fclose(fptr);
}
