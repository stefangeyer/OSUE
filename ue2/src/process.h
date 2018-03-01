/**
 * @file process.h
 * @author Stefan Geyer <e1625718 at student.tuwien.ac.at>
 * @date 03.12.2017
 *
 * @brief Subprocess header.
 *
 * This header declares the subprocess functions that are required for this task and a custom char array structure.
 **/

#ifndef UE2_PROCESS_H
#define UE2_PROCESS_H

#include <stdbool.h>

/** The maximum buffer size */
#define BUF_SIZE 1024

/**
 * This should be used when dynamically allocating a char array and you need to keep track of the array's size.
 *
 * @brief A char array and it's size mapped together.
 */
typedef struct char_array {
    char **array; /**< The dynamically allocated char array */
    int size; /**< The array's size */
} char_array_t;

/**
 * Gets the content of the given directory.
 *
 * @brief Scans the directory using the 'ls' system binary.
 * @details Should use the program options '-1a' for easier processing.
 * @param directory The directory to scan
 * @return A char array containing the directory content
 */
char_array_t *list_directory(char *directory);

/**
 * Gets the file info for the given file
 *
 * @brief Executes the system binary 'file' to get a file's type description
 * @details May exit via error_exit defined in main.h
 * @param directory The file's directory
 * @param file The file
 * @return The file info string
 */
char *file_info(char *directory, char *file);

/**
 * Gets the md5sum for the given file
 *
 * @brief Executes the system binary 'md5sum' to generate a md5 hash for the given file in the given directory
 * @details May exit via error_exit defined in main.h
 * @param directory The file's directory
 * @param file The file
 * @return The md5 hash string
 */
char *md5sum(char *directory, char *file);

/**
 * Frees the given array
 *
 * @brief Frees the given char array and it's content
 * @details First frees all allocated entries of the array, the the array pointer, the structure pointer
 * @param array The array
 */
void free_array(char_array_t *array);

#endif //UE2_PROCESS_H
