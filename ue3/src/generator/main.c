#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <assert.h>
#include <memory.h>
#include <time.h>
#include "main.h"

static graph_t *parse_arguments(int argc, char *argv[]);

static void usage(void);

static void free_graph(graph_t *graph);

static void clean_up(void);

static void parse_edge(char *input, edge_t *edge);

static int parse_int(char *string);

static int *add_vertex(int vertex, int *vertices, size_t *vertc);

static int generate_solution(graph_t *graph, solution_t *solution);

static char *pgm_name;
static circular_buffer_t *buffer;
static semaphores_t *semaphores;
volatile sig_atomic_t quit = 0;

int main(int argc, char *argv[]) {
    srand(time(NULL));
    // This also handles setting pgm_name
    graph_t *graph = parse_arguments(argc, argv);

    /*if (atexit(clean_up) != 0) {
        error_exit("Cannot define cleanup function");
    }

    buffer = memory_open();
    semaphores = semaphores_open();*/

    printf("Vertices:\n");

    for (int i = 0; i < graph->vertc; i++) {
        printf("%d\n", graph->vertices[i]);
    }

    printf("Vertex count: %d\n", (int) graph->vertc);

    printf("Edges:\n");

    for (int i = 0; i < graph->edgec; i++) {
        edge_t edge = graph->edges[i];
        printf("%d-%d\n", edge.u, edge.v);
    }

    printf("Edge count: %d\n", (int) graph->edgec);

    solution_t solution;

    while (generate_solution(graph, &solution) < 0);

    printf("Solution:\n");

    for (int i = 0; i < solution.size; i++) {
        edge_t edge = solution.edges[i];
        printf("%d-%d\n", edge.u, edge.v);
    }

    printf("Solution count: %d\n", (int) solution.size);

    free_graph(graph);
}

static int generate_solution(graph_t *graph, solution_t *solution) {
    int *colors = NULL;
    int max = -1;
    int pos = 0;

    memset(solution, 0, sizeof(solution));

    for (int i = 0; i < graph->vertc; i++) {
        int vertex = graph->vertices[i];

        if (vertex > max) {
            max = vertex;
        }
        if (vertex == max) {
            colors = realloc(colors, sizeof(int) * (max + 1));
        }
        if (colors == NULL) {
            error_exit("Cannot allocate memory");
        }

        colors[vertex] = rand() % 3; // 0, 1 or 2 are the 3 colors
    }

    printf("Colors:\n");

    for (int i = 0; i < graph->vertc; i++) {
        int vertex = graph->vertices[i];
        int color = colors[vertex];
        printf("v: %d, c: %d\n", vertex, color);
    }

    printf("Colors done\n");

    for (int i = 0; i < graph->edgec; i++) {
        // Only need solutions with less than 9 edges
        if (pos == SOLUTION_MAX_EDGES) return -1;
        edge_t edge = graph->edges[i];
        // If edges are different, add to solution
        if (colors[edge.u] != colors[edge.v]) {
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
    int *vertices = malloc(sizeof(int) * posc);
    graph_t *graph = malloc(sizeof(graph_t));
    if (edges == NULL || vertices == NULL || graph == NULL) error_exit("Cannot allocate memory");

    for (int i = 0; i < posc; i++) {
        edge_t edge;
        parse_edge(argv[optind + i], &edge);
        edges[i] = edge;
        vertices = add_vertex(edge.u, vertices, &vertc);
        vertices = add_vertex(edge.v, vertices, &vertc);
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

static int *add_vertex(int vertex, int *vertices, size_t *vertc) {
    for (int i = 0; i < *vertc; i++) {
        if (vertices[i] == vertex) return vertices;
    }

    int *new_vertices = realloc(vertices, *vertc + 1);
    new_vertices[*vertc] = vertex;
    *vertc = *vertc + 1;
    return new_vertices;
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

static void clean_up(void) {
    memory_close(buffer);
    semaphores_close(semaphores);
}