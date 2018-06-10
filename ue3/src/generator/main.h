#ifndef MAIN_H
#define MAIN_H

#include "../common/common.h"

typedef struct graph {
    size_t vertc;
    size_t edgec;
    int *vertices;
    edge_t *edges;
} graph_t;

#endif
