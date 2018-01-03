#include "common.h"

authshm_t *memory_create(void) {
    authshm_t *shared;

    /* create and/or open shared memory object */
    int shmfd = shm_open(SHM_NAME, O_RDWR | O_CREAT, PERMISSION);
    if (shmfd == -1) {
        /* error */
    }

    /* extend (set size) */
    if (ftruncate(shmfd, sizeof *shared) == -1) {
        /* error */
    }

    /* map shared memory object */
    shared = mmap(NULL, sizeof *shared, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);

    if (shared == MAP_FAILED) {
        /* error */
    }

    if (close(shmfd) == -1) {
        /* error */
    }

    return shared;
}

void memory_destroy(authshm_t *shared) {
    /* unmap shared memory */
    if (munmap(shared, sizeof *shared) == -1) {
        // error
    }

    /* remove shared memory object */
    if (shm_unlink(SHM_NAME) == -1) {
        /* error */
    }
}