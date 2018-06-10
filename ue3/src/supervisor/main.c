#include <signal.h>
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "main.h"
#include "../common/common.h"

static void create_signal_handler(void);

static void handle_signal(int signal);

static void clean_up(void);

static circular_buffer_t *buffer;
static semaphores_t *semaphores;
volatile sig_atomic_t quit = 0;

int main(int argc, char *argv[]) {
    if (atexit(clean_up) != 0) {
        error_exit("Cannot define cleanup function");
    }

    create_signal_handler();

    buffer = memory_create();
    semaphores = semaphores_create();

    printf("Supervisor started. Waiting for generator results...\n");

    solution_t best;

    memset(&best, 0, sizeof(solution_t));
    best.size = SOLUTION_MAX_EDGES + 1;

    while (!quit) {
        // Wait if there are no items in the buffer
        if (sem_wait(semaphores->used) == -1) {
            // interrupted by system call?
            if (errno == EINTR) continue;
            error_exit("Semaphore wait failed");
        }

        if (sem_wait(semaphores->mutex) == -1) {
            // interrupted by system call?
            if (errno == EINTR) continue;
            error_exit("Semaphore wait failed");
        }

        solution_t solution = buffer->solutions[buffer->readpos & (BUFFER_SIZE - 1)];
        buffer->readpos++;

        // Increment the count of the number of spaces
        if (sem_post(semaphores->free) == -1) error_exit("Semaphore post failed");

        if (solution.size < best.size) {
            if (solution.size == 0) {
                printf("The graph is 3-colorable!\n");
                quit = 1;
            } else {
                printf("Solution with %d edges: ", (int) solution.size);
                for (int i = 0; i < solution.size; i++) {
                    edge_t edge = solution.edges[i];
                    printf("%d-%d ", edge.u, edge.v);
                }
                printf("\n");
            }
            best = solution;
        }

        // Graph is either 3-colorable or a signal was received
        if (quit) {
            // Tell generators to shut down
            buffer->quit = true;
        }

        if (sem_post(semaphores->mutex) == -1) error_exit("Semaphore post failed");
    }

    return EXIT_SUCCESS;
}

static void clean_up(void) {
    memory_destroy(buffer);
    semaphores_destroy(semaphores);
}

static void handle_signal(int signal) {
    quit = 1;
}

static void create_signal_handler(void) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));

    sa.sa_handler = handle_signal;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
}

