#ifndef PROCESS_H
#define PROCESS_H

#include <stdbool.h>

#define BUF_SIZE 1024

typedef struct array_struct {
    char **array;
    int size;
} array_t;

array_t *list_directory(char *directory);

void free_array(array_t *array);

#endif //PROCESS_H
