#ifndef UE3_SERVER_H
#define UE3_SERVER_H

#include <stdlib.h>
#include "../common/common.h"

typedef struct node {
    char *username;
    char *password;
    char *message;
    struct node *next;
} node_t;

/**
 * Allows easy list extension
 *
 * @brief Appends the child node at the end of the list
 * @details If the head node is null, the
 * @param head The list's head node
 * @param child The node to add
 * @return The lists head node
 */
node_t *append_node(node_t *head, node_t *child);

/**
 * Creates a new node
 *
 * @brief Allocates memory for a new node
 * @return The pointer to the new node
 */
node_t *create_node(const char *username, const char *password, const char *message);

/**
 * Frees and destroys a given node and it's children
 *
 * @brief Recursively frees all children of the given node, the frees self
 * @param node The node to destroy
 */
void destroy_node(node_t *node);

/**
 * Writes a node to the disk in CSV format
 *
 * @brief Writes the given node as CSV to the given file
 * @param node The node to write
 * @param file_name The name of the file to write to
 */
void write_node(node_t *node, char *file_name);

/**
 * Reads all records from CSV file and returns a respective node
 *
 * @brief Reads a list of records from a CSV file
 * @param file_name The file to read from
 * @return The gathered data
 */
node_t *read_node(char *file_name);

/**
 * Print function for debug purposes
 *
 * @brief Prints a node and its children to stdout
 * @param node The node to print
 */
void print_node(node_t *node);

#endif //UE3_SERVER_H
