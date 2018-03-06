#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "util.h"
#include "main.h"

size_t get_line(FILE *fp, char *line, int n) {
    if (fgets(line, n, fp) == NULL) return 0;
    else return strlen(line);
    // Cast size_t (unsigned) to int for convenient data types (Due to CLion warning)
}

void for_each_line(char *fname, int llen, void (*run)(char*, size_t)) {
    size_t len;
    char line[llen];
    FILE *fp = fopen(fname, "r");

    if (fp == NULL) {
        error_exit("Cannot open file: %s", fname);
    }

    while ((len = get_line(fp, line, llen)) > 0) {
        run(line, len);
    }

    fclose(fp);
}