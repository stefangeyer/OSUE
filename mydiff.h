#ifndef MYDIFF_H
#define MYDIFF_H
#include <stdbool.h>

typedef struct {
    unsigned int line;
    unsigned int character;
    bool has_error = false;
    char error_message[100];
} DIFF;

DIFF* difference(char* file_name_1, char* file_name_2, bool* ignore_case);

#endif // MYDIFF_H
