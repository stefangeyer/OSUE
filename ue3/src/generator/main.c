/**
 * @file main.c
 * @author Stefan Geyer <stefan.geyer@student.tuwien.ac.at>
 * @date 10.06.2016
 *
 * @brief Main program module.
 *
 * Generator code. Parses arguments and calculates solutions.
 **/

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <assert.h>
#include <memory.h>
#include <time.h>
#include <errno.h>
#include "main.h"

static graph_t *parse_arguments(int argc, char *argv[]);

static void usage(void);

static void free_graph(graph_t *graph);

static void create_signal_handler(void);

static void handle_signal(int signal);

static void clean_up(void);

static void parse_edge(char *input, edge_t *edge);

static int parse_int(char *string);

static void add_vertex(int vertex, int *vertices, size_t *vertc);

static int generate_solution(graph_t *graph, solution_t *solution);

static char *pgm_name;
static circular_buffer_t *buffer;
static semaphores_t *semaphores;
volatile sig_atomic_t quit = 0;

int main(int argc, char *argv[]) {
    // Seed for rand operation
    srand(time(NULL));

    // This also handles setting pgm_name
    graph_t *graph = parse_arguments(argc, argv);

    if (atexit(clean_up) != 0) {
        error_exit("Cannot define cleanup function");
    }

    create_signal_handler();

    buffer = memory_open();
    semaphores = semaphores_open();

    while (!quit) {
        solution_t solution;

        // Find a valid solution
        while (generate_solution(graph, &solution) < 0);

        // wait if there is no space left
        if (sem_wait(semaphores->free) == -1) {
            // interrupted by system call?
            if (errno == EINTR) continue;
            error_exit("Semaphore wait failed");
        }

        // request synced write access
        if (sem_wait(semaphores->mutex) == -1) {
            // interrupted by system call?
            if (errno == EINTR) continue;
            error_exit("Semaphore wait failed");
        }

        // Before writing, check if supervisor has already shut down
        if (buffer->quit) {
            quit = 1;
        }

        // dont use mod but bitmask. this avoids issues with overflows of the counter
        buffer->solutions[buffer->writepos & (BUFFER_SIZE - 1)] = solution;
        buffer->writepos++;

        if (sem_post(semaphores->mutex) == -1) error_exit("Semaphore post failed");

        // increment the count of the number of items
        if (sem_post(semaphores->used) == -1) error_exit("Semaphore post failed");
    }

    printf("Generator is shutting down.\n");

    free_graph(graph);

    return EXIT_SUCCESS;
}

static int generate_solution(graph_t *graph, solution_t *solution) {
    int *colors;
    int max = -1;
    int pos = 0;

    memset(solution, 0, sizeof(solution_t));

    for (int i = 0; i < graph->vertc; i++) {
        int vertex = graph->vertices[i];

        if (vertex > max) {
            max = vertex;
        }
    }

    colors = malloc(sizeof(int) * (max + 1));

    if (colors == NULL) {
        error_exit("Cannot allocate memory");
    }

    for (int i = 0; i < graph->vertc; i++) {
        int vertex = graph->vertices[i];
        colors[vertex] = rand() % 3; // 0, 1 or 2 are the 3 colors
    }

    for (int i = 0; i < graph->edgec; i++) {
        // Only need solutions with less than 9 edges
        if (pos == SOLUTION_MAX_EDGES) return -1;
        edge_t edge = graph->edges[i];
        // If vertices of solution have the same color, add to solution
        if (colors[edge.u] == colors[edge.v]) {
            solution->edges[pos] = edge;
            pos++;
        }
    }

    solution->size = (size_t) pos;

    if (colors != NULL) free(colors);

    return 0;
}

static graph_t *parse_arguments(int argc, char *argv[]) {
    pgm_name = argv[0];

    int posc = argc - optind;

    // Check whether there was at least one edge specified
    if (posc == 0) usage();

    size_t vertc = 0, edgec = (size_t) posc;
    edge_t *edges = malloc(sizeof(edge_t) * posc);
    int *vertices = malloc(sizeof(int) * 2 * posc); // there are at max 2 * edges vertices
    graph_t *graph = malloc(sizeof(graph_t));
    if (edges == NULL || vertices == NULL || graph == NULL) error_exit("Cannot allocate memory");

    for (int i = 0; i < posc; i++) {
        edge_t edge;
        parse_edge(argv[optind + i], &edge);
        edges[i] = edge;

        add_vertex(edge.u, vertices, &vertc);
        add_vertex(edge.v, vertices, &vertc);
    }

    graph->vertices = vertices;
    graph->edges = edges;
    graph->vertc = vertc;
    graph->edgec = edgec;

    return graph;
}

static void parse_edge(char *input, edge_t *edge) {
    char *token, *delimiter = "-";

    token = strtok(input, delimiter);
    int u = parse_int(token);
    if (u < 0) error_exit("Invalid or negative vertex value provided for an u vertex.");

    token = strtok(NULL, delimiter);
    int v = parse_int(token);
    if (v < 0) error_exit("Invalid or negative vertex value provided for a v vertex.");

    if (strtok(NULL, delimiter) != NULL) error_exit("Vertices must be in format u-v");

    edge->u = u;
    edge->v = v;
}

static void add_vertex(int vertex, int *vertices, size_t *vertc) {
    for (int i = 0; i < *vertc; i++) {
        if (vertices[i] == vertex) return;
    }

    vertices[*vertc] = vertex;
    *vertc = *vertc + 1;
}

static int parse_int(char *string) {
    char *end;
    int result = (int) strtol(string, &end, 10);
    if (*end != 0) return -1;
    return result;
}

static void usage(void) {
    fprintf(stderr, "SYNOPSIS\n\t%s EDGE1...\nEXAMPLE\n\t%s 0-1 0-2 0-3 1-2 1-3 2-3\n", pgm_name, pgm_name);
    exit(EXIT_FAILURE);
}

static void free_graph(graph_t *graph) {
    free(graph->edges);
    free(graph->vertices);
    free(graph);
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

static void clean_up(void) {
    memory_close(buffer);
    semaphores_close(semaphores);
}