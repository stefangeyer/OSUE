#include "mydiff.h"
#include <ctype.h>

static int get_line(FILE *fp, char *line) {
    if (fgets(line, LINE_MAX, fp) == NULL) return 0;
    else return strlen(line);
}

static int min(int a, int b) {
    return a < b ? a : b;
}

void difference(FILE *file1, FILE *file2, bool ignore_case) {
    char line1[LINE_MAX], line2[LINE_MAX];
    int len1, len2;
    unsigned int line_nr = 1;
    bool found_diff = false;

    while ((len1 = get_line(file1, line1)) > 0 && (len2 = get_line(file2, line2)) > 0) {
        unsigned int diff_amt = 0;
        // for loop only runs until the shorter line reaches its end
        for (int i = 0; i < min(len1, len2) - 1; i++) {
            char c1 = ignore_case ? (char) tolower(line1[i]) : line1[i];
            char c2 = ignore_case ? (char) tolower(line2[i]) : line2[i];
            if (c1 != c2) diff_amt++;
        }

        if (diff_amt > 0) {
            printf("Line: %i, Characters: %u\n", line_nr, diff_amt);
            found_diff = true;
        }

        line_nr++;
    }

    if (!found_diff) printf("File contents are identical");
}