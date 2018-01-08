/**
 * @file main.h
 * @author Stefan Geyer <stefan.geyer@student.tuwien.ac.at>
 * @date 08.01.2017
 *
 * @brief Main program module header.
 *
 * This file only exists to have a separate prototype.
 **/

#ifndef UE3_MAIN_H
#define UE3_MAIN_H

#include "client.h"

#define MODE_REGISTER 0 /**< Client is in register mode **/
#define MODE_LOGIN 1 /**< Client is in login mode **/

/**
 * Mandatory usage function.
 * @brief This function writes helpful usage information about the program to stderr.
 * @details global variables: pgm_name
 */
static void usage(void);

/**
 * @brief Takes the argument vector and parses the positional arguments and options
 * @details Sets the global variables username, password and mode
 *
 * @param argc The argument count
 * @param argv The argument vector
 */
static void parse_arguments(int argc, char *argv[]);

/**
 * @brief Starts the repeating handshake between client and server
 * @details Reads command from stdout, then executes it.
 */
static void interact(void);

/**
 * @brief Contains the communication via semaphores.
 * @details Calls the given behaviour for request and response handling.
 *
 * @param handle_request The request behaviour
 * @param handle_response The response behaviour
 * @return CONV_CODE_SUCCESS (0) on operation success, CONV_CODE_FAILURE (-1) on opperation failure,
 *         CONV_CODE_INVALID_SESSION (-2) on invalid session and CONV_CODE_INVALID_RESPONSE (-3) on invalid response code
 */
static int communicate(
        void (*handle_request)(auth_memory_t *shared, char *username, char *password, char *session, char *secret),
        int (*handle_response)(auth_memory_t *shared, char *username, char *password, char *session, char *secret)
);

/**
 * @brief Cleans up existing resources
 * @brief should be called with atexit
 */
static void clean_up(void);

#endif //UE3_MAIN_H
