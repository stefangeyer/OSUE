#ifndef UE1A_UTIL_H
#define UE1A_UTIL_H

/**
 * Read a line from the given file
 * @brief This function uses fgets to read the next line or at most LINE_MAX characters from the given file.
 * @details When fgets returns NULL, this function returns 0 and otherwise the line length.
 * @param fp file to read from
 * @param line line handle
 * @param n will read at most n-1 chars
 * @return Line length
 */
size_t get_line(FILE *fp, char *line, int n);

void for_each_line(char *fname, int llen, void (*run)(char*, size_t));

#endif //UE1A_UTIL_H
