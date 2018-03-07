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

char *read_file(char *file, int n) {
    size_t len = 0, clen = 0;
    char line[n], *result = NULL;
    FILE *fp = fopen(file, "r");

    if (fp == NULL) {
        error_exit("Cannot open file: %s", file);
    }

    while ((len = get_line(fp, line, n)) > 0) {
        clen += len + 1;
        if (result == NULL) {
            result = malloc(clen);
            memset(result, 0, clen);
        } else {
            result = realloc(result, clen);
        }
        strcat(result, line);
    }

    fclose(fp);
    return result;
}