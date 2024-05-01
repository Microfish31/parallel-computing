#include <stdio.h>
#include <omp.h>
int main(){

    omp_set_num_threads(16); // null is default value

    // Do this part in parallel
    #pragma omp parallel
    {
      printf( "Hello  world!\n" );
    }

    return 0;
}

