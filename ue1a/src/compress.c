#include <string.h>
#include <stdio.h>
#include "compress.h"

int strcmpr(const char *src, char *res, size_t n) {
    int cnt = 0;
    char prev = src[0];
    memset(res, 0, n);

    for (int i = 0; src[i] != '\0'; i++) {
        if (src[i] == prev) cnt++;
        else {
            snprintf(res, n, "%s%c%d", res, prev, cnt);
            cnt = 1;
            prev = src[i];
        }
    }

    snprintf(res, n, "%s%c%d", res, prev, cnt);

    return (int) strlen(res);
}