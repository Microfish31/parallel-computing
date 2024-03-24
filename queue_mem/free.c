#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

int main() {
    const char *shm_buffer_name = "shm_buffer";
    const char *shm_in_name = "shm_in";
    const char *shm_out_name = "shm_out";
    const char *shm_flag_name = "shm_flag";

    /* Delete shared memory objects */
    shm_unlink(shm_buffer_name);
    shm_unlink(shm_in_name);
    shm_unlink(shm_out_name);
    shm_unlink(shm_flag_name);

    return 0;
}
