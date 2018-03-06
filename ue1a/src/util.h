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
 * Execute code for each line of a file
 * @brief Reads each line from the file with the given name and calls the given function with the line's content
 * @details Opens and closes the file with the given name, each line may at most be n chars long
 * @param fname The name of the file to process
 * @param n A line's max length
 * @param run This function is called for each line. the 1st param represents the lines content, the 2nd its length.
 */
void for_each_line(char *fname, int n, void (*run)(char*, size_t));

#endif //UE1A_UTIL_H
