#ifndef UE3_MAIN_H
#define UE3_MAIN_H

#include "server.h"

/**
 * @brief Clean up function that frees or closes allocated resources
 * @details Should be called with atexit
 */
static void clean_up();

/**
 * Mandatory usage function.
 * @brief This function writes helpful usage information about the program to stderr.
 * @details global variables: pgm_name
 */
static void usage(void);

/**
 * Takes argc and argv from the main function and sets required properties
 *
 * @brief Parses options and makes sure there are no positional arguments
 * @details Parses host and port; both can occur once at max.
 * @param argc arg count
 * @param argv arg vector
 */
static void parse_arguments(int argc, char *argv[]);

/**
 * @brief This function interacts with the shared memory to perform the login
 * @details Creates a session id on success and should use the macros RESPONSE_LOGIN_SUCCESS or RESPONSE_LOGIN_FAILURE as state
 */
static void attempt_login();

/**
 * @brief This function interacts with the shared memory to perform the registration
 * @details Creates a new user on success and should use the macros RESPONSE_REGISTER_SUCCESS or RESPONSE_REGISTER_FAILURE as state
 */
static void attempt_register();

/**
 * @brief This function interacts with the shared memory and reads a users secret
 * @details Reads the user's secret on success and should use the macros RESPONSE_READ_SUCCESS or RESPONSE_READ_FAILURE as state
 */
static void attempt_read();

/**
 * @brief This function interacts with the shared memory and overwrites a users secret
 * @details Overwrites the user's secret on success and should use the macros RESPONSE_WRITE_SUCCESS or RESPONSE_WRITE_FAILURE as state
 */
static void attempt_write();

/**
 * @brief This function interacts with the shared memory and performs the logout for the user
 * @details Destroys the user's session on success and should use the macros RESPONSE_LOGOUT_SUCCESS or RESPONSE_LOGOUT_FAILURE as state
 */
static void attempt_logout();

#endif //UE3_MAIN_H
