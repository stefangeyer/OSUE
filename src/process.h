#ifndef PROCESS_H
#define PROCESS_H

#include <stdbool.h>

#define BUF_SIZE 1024

typedef struct char_array {
    char **array;
    int size;
} char_array_t;

char_array_t *list_directory(char *directory);

char *file_info(char *directory, char *file);

char *md5sum(char *directory, char *file);

void free_array(char_array_t *array);

#endif //PROCESS_H
