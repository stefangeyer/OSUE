/**
 * @file main.c
 * @author Stefan Geyer <stefan.geyer@student.tuwien.ac.at>
 * @date 10.06.2018
 *
 * @brief Main program module for generators.
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

static char *pgm_name; /** Program name */
static circular_buffer_t *buffer; /** Shared memory circular buffer */
static semaphores_t *semaphores; /** Semaphores clustered together */
volatile sig_atomic_t quit = 0; /** Quit flag */

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

/**
 * @brief Generate a solution for the input graph
 * @details Calculates colors for each vertex and revoces edges where both vertices have the same color
 * @param graph The graph to find a solution for
 * @param solution The solution
 * @return Result code; -1 if the found solution has too many edges
 */
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

/**
 * @brief Takes the argument vector and parses the positional arguments and options
 * @details Creates a graph instance from the given input
 *
 * @param argc The argument count
 * @param argv The argument vector
 * @return The created graph
 */
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

/**
 * @brief Parses an edge from an input string
 * @details Uses the declared util functions
 * @param input The string to parse
 * @param edge The resulting edge
 */
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

/**
 * @brief Adds a vertex to the vertices array but checks for duplicates
 * @details Also increases the vertex counter
 * @param vertex The vertex to add
 * @param vertices The array
 * @param vertc The counter
 */
static void add_vertex(int vertex, int *vertices, size_t *vertc) {
    for (int i = 0; i < *vertc; i++) {
        if (vertices[i] == vertex) return;
    }

    vertices[*vertc] = vertex;
    *vertc = *vertc + 1;
}

/**
 * @brief Parses a number from a string
 * @details Uses strtol
 * @param string The string to parse
 * @return The parsed number or -1 on error
 */
static int parse_int(char *string) {
    char *end;
    int result = (int) strtol(string, &end, 10);
    if (*end != 0) return -1;
    return result;
}

/**
 * Mandatory usage function.
 * @brief This function writes helpful usage information about the program to stderr.
 * @details global variables: pgm_name
 */
static void usage(void) {
    fprintf(stderr, "SYNOPSIS\n\t%s EDGE1...\nEXAMPLE\n\t%s 0-1 0-2 0-3 1-2 1-3 2-3\n", pgm_name, pgm_name);
    exit(EXIT_FAILURE);
}

static void free_graph(graph_t *graph) {
    free(graph->edges);
    free(graph->vertices);
    free(graph);
}

/**
 * The callback function for the signal handler.
 * Stops the main loop.
 *
 * @param signal The signal to handle
 */
static void handle_signal(int signal) {
    quit = 1;
}

/**
 * @brief Sets up the signal handler
 * @details Handle signals SIGINT and SIGTERM
 */
static void create_signal_handler(void) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));

    sa.sa_handler = handle_signal;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
}

/**
 * @brief Cleans up existing resources
 * @brief should be called with atexit
 */
static void clean_up(void) {
    memory_close(buffer);
    semaphores_close(semaphores);
}