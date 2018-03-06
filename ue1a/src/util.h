/**
 * @file util.h
 * @author Stefan Geyer <stefan.geyer@student.tuwien.ac.at>
 * @date 06.03.2018
 *
 * @brief Util header.
 *
 * Defines utility functions.
 **/

#ifndef UE1A_UTIL_H
#define UE1A_UTIL_H

/**
 * Read a line from the given file
 * @brief This function uses fgets to read the next line or at most n characters from the given file.
 * @details When fgets returns NULL, this function returns 0 and otherwise the line length.
 * @param fp file to read from
 * @param line line handle
 * @param n will read at most n-1 chars
 * @return Line length
 */
size_t get_line(FILE *fp, char *line, int n);

/**
 * Read a files content into a string.
 * @brief Opens the file with the given name and reads up to n chars from each line and returns the result
 * @details String is allocated. Therefore needs to be freed by the user
 * @param file The name of the file to open
 * @param n A line's max length
 * @return Pointer to the allocated string
 */
char *read_file(char *file, int n);

#endif //UE1A_UTIL_H
