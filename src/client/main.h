#ifndef UE3_MAIN_H
#define UE3_MAIN_H

#include "client.h"

#define MODE_REGISTER 0
#define MODE_LOGIN 1

/**
 * Mandatory usage function.
 * @brief This function writes helpful usage information about the program to stderr.
 * @details global variables: pgm_name
 */
static void usage(void);

static void parse_arguments(int argc, char *argv[]);

static void interact(void);

static void clean_up(void);

#endif //UE3_MAIN_H
