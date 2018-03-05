#include <stdio.h>
#include <string.h>
#include "util.h"

int get_line(FILE *fp, char *line, int n) {
    if (fgets(line, n, fp) == NULL) return 0;
    else return (int) strlen(line);
    // Cast size_t (unsigned) to int for convenient data types (Due to CLion warning)
}