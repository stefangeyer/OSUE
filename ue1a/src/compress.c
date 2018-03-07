/**
 * @file compress.c
 * @author Stefan Geyer <stefan.geyer@student.tuwien.ac.at>
 * @date 06.03.2018
 *
 * @brief Compression module.
 *
 * Provides compression functions.
 **/

#include <string.h>
#include <stdio.h>
#include "compress.h"

int strncmpr(const char *src, char *res, size_t n) {
    int cnt = 0, off = 0;
    char prev = src[0];
    memset(res, 0, n);

    for (int i = 0; src[i] != '\0'; i++) {
        if (src[i] == '\r') continue; // fix for cygwin
        else if (src[i] == prev) cnt++;
        else {
            off += snprintf(res + off, n, "%c%d", prev, cnt);
            cnt = 1;
            prev = src[i];
        }
    }

    off += snprintf(res + off, n, "%c%d", prev, cnt);

    return off;
}