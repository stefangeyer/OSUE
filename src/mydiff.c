/**
 * @file mydiff.c
 * @author Stefan Geyer <stefan.geyer@student..tuwien.ac.at>
 * @date 16.10.2017
 *
 * @brief implementation of the mydiff module
 *
 * This implementation uses ctype.h for tolower and string.h for strlen
 *
 */

#include "mydiff.h"
#include <ctype.h>
#include <string.h>

/**
 * Read a line from the given file
 * @brief This function uses fgets to read the next line or at most LINE_MAX characters from the given file.
 * @details When fgets returns NULL, this function returns 0 and otherwise the line length.
 * @param fp file to read from
 * @param line line handle
 * @return Line length
 */
static int get_line(FILE *fp, char line[]) {
    if (fgets(line, LINE_MAX, fp) == NULL) return 0;
    else return strlen(line);
}

/**
 * Gets the lesser of two ints
 * @brief Compares two ints and returns the first, if it is less than the second
 * @param a First number
 * @param b Second number
 * @return The smaller number
 */
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
        // for loop only runs until the shorter line reaches its end.
        // min function prevents infinite loop (line contains no \n)
        for (int i = 0; i < min(len1, len2); i++) {
            char c1 = ignore_case ? (char) tolower(line1[i]) : line1[i];
            char c2 = ignore_case ? (char) tolower(line2[i]) : line2[i];
            // Only compare until either line reaches it's end
            if (c1 == '\n' || c2 == '\n') break;
            if (c1 != c2) diff_amt++;
        }

        if (diff_amt > 0) {
            printf("Line: %i, Characters: %u\n", line_nr, diff_amt);
            found_diff = true;
        }

        line_nr++;
    }

    if (!found_diff) printf("No character mismatch found");
}