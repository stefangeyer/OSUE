#ifndef MAIN_H
#define MAIN_H

#include "../common/common.h"

/**
 * @struct Graph struct
 * @brief Parsed data from a generator
 * @details Typedef as graph_t
 */
typedef struct graph {
    size_t vertc; /** vertex count */
    size_t edgec; /** edge count */
    int *vertices; /** vertices */
    edge_t *edges; /** edges */
} graph_t;

#endif
