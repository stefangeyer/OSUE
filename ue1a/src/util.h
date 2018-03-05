#ifndef UE1A_UTIL_H
#define UE1A_UTIL_H

#define LINE_MAX 100 /**< Lines ready by this program should not be longer than the specified amount */

/**
 * Read a line from the given file
 * @brief This function uses fgets to read the next line or at most LINE_MAX characters from the given file.
 * @details When fgets returns NULL, this function returns 0 and otherwise the line length.
 * @param fp file to read from
 * @param line line handle
 * @param n will read at most n-1 chars
 * @return Line length
 */
int get_line(FILE *fp, char *line, int n);

#endif //UE1A_UTIL_H
