#include <signal.h>
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
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

    while (!quit) {

    }

    printf("Supervisor is shutting down.\n");

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

