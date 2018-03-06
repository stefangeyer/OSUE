/**
 * @file util.c
 * @author Stefan Geyer <stefan.geyer@student.tuwien.ac.at>
 * @date 06.03.2018
 *
 * @brief Util module.
 *
 * Provides utility functions.
 **/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "util.h"
#include "main.h"

size_t get_line(FILE *fp, char *line, int n) {
    if (fgets(line, n, fp) == NULL) return 0;
    else return strlen(line);
}

void for_each_line(char *fname, int n, void (*run)(char*, size_t)) {
    size_t len;
    char line[n];
    FILE *fp = fopen(fname, "r");

    if (fp == NULL) {
        error_exit("Cannot open file: %s", fname);
    }

    while ((len = get_line(fp, line, n)) > 0) {
        run(line, len);
    }

    fclose(fp);
}