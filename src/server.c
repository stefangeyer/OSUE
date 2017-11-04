/**
 * @file server.c
 * @author OSUE Team <osue-team@cps.tuwien.ac.at>
 * @date 2017-10-06
 *
 * @brief Server for OSUE exercise 1B `Battleship'.
 */

// IO, C standard library, POSIX API, data types:
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

// Assertions, errors, signals:
#include <assert.h>
#include <errno.h>
#include <signal.h>

// Time:
#include <time.h>

// Sockets, TCP, ... :
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>

// stuff shared by client and server:
#include "common.h"

// Static variables for things you might want to access from several functions:
static const char *port = DEFAULT_PORT; // the port to bind to

// Static variables for resources that should be freed before exiting:
static struct addrinfo *ai = NULL;      // addrinfo struct
static int sockfd = -1;                 // socket file descriptor
static int connfd = -1;                 // connection file descriptor

/* TODO
 * You might want to add further static variables here, for instance to save
 * the programe name (argv[0]) since you should include it in all messages.
 *
 * You should also have some kind of a list which saves information about your
 * ships. For this purpose you might want to define a struct. Bear in mind that
 * your server must keep record about which ships have been hit (and also where
 * they have been hit) in order to know when a ship was sunk.
 *
 * You might also find it convenient to add a few functions, for instance:
 *  - a function which cleans up all resources and exits the program
 *  - a function which parses the arguments from the command line
 *  - a function which adds a new ship to your list of ships
 *  - a function which checks whether a client's shot hit any of your ships
 */

static char *pgm_name; /**< The program name */

/**
 * Mandatory usage function.
 * @brief This function writes helpful usage information about the program to stderr.
 * @details global variables: pgm_name
 */
void usage(void) {
    fprintf(stderr, "Server:\n\tSYNOPSIS\n\t\t%s [-p PORT] SHIP1...\n\tEXAMPLE\n"
            "\t\t%s -p 1280 C2E2 F0H0 B6A6 E8E6 I2I5 H8I8\n", pgm_name, pgm_name);
    exit(EXIT_FAILURE);
}

/**
 * Prints anerror message to stderr. Should only be called whenever an error occurs
 * shall exit with failure.
 * @brief This function prints an error message to stderr and exits with EXIT_FAILURE.
 * @details global variables: pgm_name
 * @param message The message that shall be printed before exiting
 */
void exitError(char *message, bool showUsage) {
    fprintf(stderr, "%s: An error has occurred: %s\n", pgm_name, message);
    if (showUsage) usage();
    else exit(EXIT_FAILURE);
}

void parseArguments(int argc, char *argv[]) {
    pgm_name = argv[0];

    int c;
    int opt_p = 0;
    bool has_invalid_option = false;

    // 3rd param: optstring defines legitimate option characters.
    // Appending : to an argument character implicates that the option requires an argument
    while ((c = getopt(argc, argv, "p:")) != -1) {
        switch (c) {
            case 'p':
                // Option p is optional and may occur once.
                opt_p++;
                port = optarg;
                break;
            case '?':
                has_invalid_option = true;
                break;
            default:
                assert(0);
        }
    }

    if (opt_p >= 1) exitError("Port option can be set only once", true);

    // Check whether there are the correct amount of positional arguments
    // or an in valid option were supplied.
    if ((argc - optind) != 2 || has_invalid_option) usage();

}

int main(int argc, char *argv[]) {
    // This also handles setting pgm_name
    parseArguments(argc, argv);

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    // Returns 0 or error code but does not set errno
    int res = getaddrinfo(NULL, port, &hints, &ai);
    if (res != 0) exitError((char *) gai_strerror(res), false);

    sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
    if (sockfd < 0) exitError(strerror(errno), false);

    int val = 1;
    res = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof val);
    if (res) exitError(strerror(errno), false);

    res = bind(sockfd, ai->ai_addr, ai->ai_addrlen);
    if (res) exitError(strerror(errno), false);

    res = listen(sockfd, 1);
    if (res) exitError(strerror(errno), false);

    connfd = accept(sockfd, NULL, NULL);
    if (connfd) exitError(strerror(errno), false);

    /* TODO
     * Here you might want to add variables to keep track of the game status,
     * for instance the number of rounds that have been played.
     */

    while (...) {
        /* TODO
         * add code to:
         *  - wait for a request from the client
         *  - check for a parity error or invalid coordinates in the request
         *  - check whether a ship was hit and determine the status to return
         *  - send an according response to the client
         */
    }

    /* TODO
     * cleanup
     */
    freeaddrinfo(ai);
}
