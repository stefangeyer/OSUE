#ifndef UE1A_MAIN_H
#define UE1A_MAIN_H

#define LINE_LENGTH 100 /**< Lines ready by this program should not be longer than the specified amount */

/**
 * @brief Prints the given reason to stderr and exits with EXIT_FAILURE
 * @param format The reason's format
 * @param ... format params
 */
void error_exit(char *format, ...);

#endif //UE1A_MAIN_H
