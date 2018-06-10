/**
 * @file common.c
 * @author Stefan Geyer <stefan.geyer@student.tuwien.ac.at>
 * @date 08.01.2017
 *
 * @brief Common program module.
 *
 * This file implements all function used by server AND client
 **/

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include "common.h"

circular_buffer_t *memory_create(void) {
    circular_buffer_t *shared;

    /* create and/or open shared memory object */
    int shmfd = shm_open(SHM_NAME, O_RDWR | O_CREAT, PERMISSION);
    if (shmfd == -1) {
        error_exit("Cannot create shared memory");
    }

    /* extend (set size) */
    if (ftruncate(shmfd, sizeof *shared) == -1) {
        error_exit("Cannot truncate shared memory file");
    }

    /* map shared memory object */
    shared = mmap(NULL, sizeof *shared, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);

    if (shared == MAP_FAILED) {
        error_exit("Cannot map shared memory");
    }

    if (close(shmfd) == -1) {
        error_exit("Cannot close shared memory file");
    }

    return shared;
}

void memory_destroy(circular_buffer_t *shared) {
    if (shared == NULL) return;
    /* unmap shared memory */
    if (munmap(shared, sizeof *shared) == -1) {
        fprintf(stderr, "Cannot unmap shared memory"); // Dont error_exit, since this is will be called with atexit
    }

    /* remove shared memory object */
    if (shm_unlink(SHM_NAME) == -1) {
        fprintf(stderr, "Cannot unlink shared memory"); // Dont error_exit, since this is will be called with atexit
    }
}

circular_buffer_t *memory_open(void) {
    circular_buffer_t *shared;

    /* create and/or open shared memory object */
    int shmfd = shm_open(SHM_NAME, O_RDWR, 0);
    if (shmfd == -1) {
        error_exit("Cannot open shared memory. Has the server been started yet?");
    }

    /* map shared memory object */
    shared = mmap(NULL, sizeof *shared, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);

    if (shared == MAP_FAILED) {
        error_exit("Cannot map shared memory");
    }

    if (close(shmfd) == -1) {
        error_exit("Cannot close shared memory file");
    }

    return shared;
}

void memory_close(circular_buffer_t *shared) {
    if (shared == NULL) return;
    /* unmap shared memory */
    if (munmap(shared, sizeof *shared) == -1) {
        fprintf(stderr, "Cannot unmap shared memory"); // Dont error_exit, since this is will be called with atexit
    }
}

//void print_memory(circular_buffer_t *shared) {
//    printf("server_available: %d\n"
//                   "username: %s\n"
//                   "password: %s\n"
//                   "secret: %s\n"
//                   "session: %s\n"
//                   "state: %d\n",
//           shared->server_available, shared->username, shared->password, shared->secret, shared->session, shared->state);
//}

semaphores_t *semaphores_create(void) {
    semaphores_t *semaphores = malloc(sizeof(semaphores_t));

    if (semaphores == NULL) {
        error_exit("Cannot allocate memory");
        return NULL;
    }

    if ((semaphores->free = sem_open(SEM_FREE, O_CREAT, PERMISSION, BUFFER_SIZE)) == SEM_FAILED) {
        error_exit("Cannot create free semaphore");
    }

    if ((semaphores->used = sem_open(SEM_USED, O_CREAT, PERMISSION, 0)) == SEM_FAILED) {
        error_exit("Cannot create used semaphore");
    }

    if ((semaphores->mutex = sem_open(SEM_MUTEX, O_CREAT, PERMISSION, 1)) == SEM_FAILED) {
        error_exit("Cannot create mutex semaphore");
    }

    return semaphores;
}

void semaphores_destroy(semaphores_t *semaphores) {
    if (semaphores == NULL) return;

    if (sem_close(semaphores->free) < 0) {
        fprintf(stderr, "Cannot close free semaphore"); // Dont error_exit, since this is will be called with atexit
    }

    if (sem_close(semaphores->used) < 0) {
        fprintf(stderr, "Cannot close used semaphore"); // Dont error_exit, since this is will be called with atexit
    }

    if (sem_close(semaphores->mutex) < 0) {
        fprintf(stderr, "Cannot close mutex semaphore"); // Dont error_exit, since this is will be called with atexit
    }

    if (sem_unlink(SEM_FREE) < 0) {
        fprintf(stderr, "Cannot unlink free semaphore"); // Dont error_exit, since this is will be called with atexit
    }

    if (sem_unlink(SEM_USED) < 0) {
        fprintf(stderr, "Cannot unlink used semaphore"); // Dont error_exit, since this is will be called with atexit
    }

    if (sem_unlink(SEM_MUTEX) < 0) {
        fprintf(stderr, "Cannot unlink mutex semaphore"); // Dont error_exit, since this is will be called with atexit
    }

    free(semaphores);
}

semaphores_t *semaphores_open(void) {
    semaphores_t *semaphores = malloc(sizeof(semaphores_t));

    if (semaphores == NULL) {
        error_exit("Cannot allocate memory");
        return NULL;
    }

    if ((semaphores->free = sem_open(SEM_FREE, 0)) == SEM_FAILED) {
        error_exit("Cannot open free semaphore");
    }

    if ((semaphores->used = sem_open(SEM_USED, 0)) == SEM_FAILED) {
        error_exit("Cannot open used semaphore");
    }

    if ((semaphores->mutex = sem_open(SEM_MUTEX, 1)) == SEM_FAILED) {
        error_exit("Cannot open mutex semaphore");
    }

    return semaphores;
}

void semaphores_close(semaphores_t *semaphores) {
    if (semaphores == NULL) return;

    if (sem_close(semaphores->free) < 0) {
        fprintf(stderr, "Cannot close free semaphore"); // Dont error_exit, since this is will be called with atexit
    }

    if (sem_close(semaphores->used) < 0) {
        fprintf(stderr, "Cannot close used semaphore"); // Dont error_exit, since this is will be called with atexit
    }

    if (sem_close(semaphores->mutex) < 0) {
        fprintf(stderr, "Cannot close mutex semaphore"); // Dont error_exit, since this is will be called with atexit
    }

    free(semaphores);
}

void error_exit(char *reason) {
    fprintf(stderr, "An error occurred: %s\n", reason);
    exit(EXIT_FAILURE);
}