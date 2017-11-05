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

static char *pgm_name;

static const char *host = DEFAULT_HOST; // the host to connect to
static const char *port = DEFAULT_PORT; // the port to connect to

static struct addrinfo *ai = NULL;      // addrinfo struct
static int sockfd = -1;                 // socket file descriptor

/**
 * Mandatory usage function.
 * @brief This function writes helpful usage information about the program to stderr.
 * @details global variables: pgm_name
 */
static void usage(void) {
    fprintf(stderr, "SYNOPSIS\n\t%s [-h HOSTNAME] [-p PORT]\n", pgm_name);
    exit(EXIT_FAILURE);
}

static void clean_up(void) {
    close(sockfd);
    freeaddrinfo(ai);
}

/**
 * Prints an error message to stderr. Should only be called whenever an error occurs
 * shall exit with failure.
 * @brief This function prints an error message to stderr and exits with EXIT_FAILURE.
 * @details global variables: pgm_name
 * @param message The message that shall be printed before exiting
 */
static void error_exit(char *message, bool show_usage) {
    fprintf(stderr, "[%s] ERROR: %s\n", pgm_name, message);
    clean_up();
    if (show_usage) usage();
    else exit(EXIT_FAILURE);
}

static void parse_arguments(int argc, char *argv[]) {
    pgm_name = argv[0];

    int c;
    int opt_h = 0, opt_p = 0;
    bool has_invalid_option = false;

    // 3rd param: optstring defines legitimate option characters.
    // Appending : to an argument character implicates that the option requires an argument
    while ((c = getopt(argc, argv, "h:p:")) != -1) {
        switch (c) {
            case 'h':
                // Option h is optional and may occur once.
                opt_h++;
                host = optarg;
                break;
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

    if (opt_h > 1) error_exit("Host option can only be set once.", true);
    if (opt_p > 1) error_exit("Port option can only be set once.", true);

    // Check whether there are the correct amount of positional arguments
    // or an invalid option were supplied.
    if ((argc - optind) > 0 || has_invalid_option) usage();
}

static void dumb_send(void) {
    // Cite Handout: "A dumb strategy (e.g. repeatedly shooting at the same square) is sufficient"
    // Dumb strategy: Always hit the same field: x = 2, y = 3
    int coord = 2 + 3 * 10;
    int parity = calculate_parity((char) coord, 6);
    // shift parity to index 7 and combine both
    char out = (char) (coord | (parity << 7));
    if (send(sockfd, &out, 1, MSG_WAITALL) < 0)
        error_exit(strerror(errno), false);
}

int main(int argc, char *argv[]) {
    parse_arguments(argc, argv);

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    // Returns 0 or error code but does not set errno
    int res = getaddrinfo(host, port, &hints, &ai);
    if (res != 0) error_exit((char *) gai_strerror(res), false);

    sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
    if (sockfd < 0) error_exit(strerror(errno), false);

    res = connect(sockfd, ai->ai_addr, ai->ai_addrlen);
    if (res < 0) error_exit(strerror(errno), false);

    int result = EXIT_SUCCESS;
    bool game_over = false;
    ssize_t recv_size;
    char in;
    dumb_send();

    while (!game_over && (recv_size = recv(sockfd, &in, 1, MSG_WAITALL)) > 0) {
        int hit = in & 3; // mask = 11
        int status = (in & 12) >> 2; // mask = 1100; shift twice to the right

        switch (status) {
            case 0:
                // Game is going on
                // With a smart strategy, hit value would be saved
                break;
            case 1:
                // Game over; check hit to determine if client won; return value was already set
                if (hit == 3) printf("I win :)");
                else printf("game lost");
                game_over = true;
                break;
            case 2:
                // Parity error
                fprintf(stderr, "parity error");
                game_over = true;
                result = status;
            case 3:
                // Invalid coordinate
                fprintf(stderr, "invalid coordinate");
                game_over = true;
                result = status;
            default:
                assert(0);
        }
    }

    // 0 == orderly shutdown; -1 == error
    if (recv_size < 0) error_exit(strerror(errno), false);

    clean_up();
    return result;
}