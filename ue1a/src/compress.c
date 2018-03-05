#include <string.h>
#include <stdio.h>
#include "compress.h"

int strcmpr(const char *src, char *res, size_t n) {
    int cnt = 0, off = 0;
    char prev = src[0];
    memset(res, 0, n);

    for (int i = 0; src[i] != '\0'; i++) {
        if (src[i] == prev) cnt++;
        else {
            off += snprintf(res + off, n, "%c%d", prev, cnt);
            cnt = 1;
            prev = src[i];
        }
    }

    off += snprintf(res + off, n, "%c%d", prev, cnt);

    return off;
}