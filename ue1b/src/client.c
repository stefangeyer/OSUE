/**
 * @file client.c
 * @author Stefan Geyer <stefan.geyer@student.tuwien.ac.at>
 * @date 09.04.2018
 *
 * @brief Client program module.
 *
 * The client tries to connect to the server and attempts to win the battleship game.
 * This implementation uses a dumb algorithm.
 **/

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

static char *pgm_name; /**< The program name.*/

static struct addrinfo *ai = NULL;      /**< addrinfo struct */
static int sockfd = -1;                 /**< socket file descriptor */

/**
 * Mandatory usage function.
 * @brief This function writes helpful usage information about the program to stderr.
 * @details global variables: pgm_name
 */
static void usage(void) {
    fprintf(stderr, "SYNOPSIS\n\t%s [-h HOSTNAME] [-p PORT]\n", pgm_name);
    exit(EXIT_FAILURE);
}

/**
 * Cleans up before the program exits
 * @brief Coses socket connections and frees the addrinfo
 * @details Frees sockfd and ai
 */
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
 * @param show_usage Display usage before exiting
 */
static void error_exit(char *message, bool show_usage) {
    fprintf(stderr, "[%s] ERROR: %s\n", pgm_name, message);
    clean_up();
    if (show_usage) usage();
    else exit(EXIT_FAILURE);
}

/**
 * Takes argc and argv from the main function and sets required properties
 *
 * @brief Parses options and makes sure there are no positional arguments
 * @details Parses host and port; both can occur once at max.
 * @param argc arg count
 * @param argv arg vector
 * @param host Pointer to the host string
 * @param port Pointer to the port string
 */
static void parse_arguments(int argc, char *argv[], char **host, char **port) {
    pgm_name = argv[0];

    int c;
    int opt_h = 0, opt_p = 0;
    bool invopt = false;

    // 3rd param: optstring defines legitimate option characters.
    // Appending : to an argument character implicates that the option requires an argument
    while ((c = getopt(argc, argv, "h:p:")) != -1) {
        switch (c) {
            case 'h':
                // Option h is optional and may occur once.
                opt_h++;
                *host = optarg;
                break;
            case 'p':
                // Option p is optional and may occur once.
                opt_p++;
                *port = optarg;
                break;
            case '?':
                invopt = true;
                break;
            default:
                assert(0);
        }
    }

    if (opt_h > 1) error_exit("Host option can only be set once.", true);
    if (opt_p > 1) error_exit("Port option can only be set once.", true);

    // Check whether there are the correct amount of positional arguments
    // or an invalid option were supplied.
    if ((argc - optind) > 0 || invopt) usage();
}

/**
 * Generates the values hit and status from the given response buffer
 *
 * @brief Generates hit and status from buffer using bit masks
 * @details hit mask: 11, status mask: 1100 + shift
 * @param buffer The buffer to extract the values from
 * @param hit Pointer to the hit value
 * @param status Pointer to the status value
 */
static void parse_response(uint8_t buffer, uint8_t *hit, uint8_t *status) {
    *hit = (uint8_t) (buffer & 3); // mask = 11
    *status = (uint8_t) ((buffer & 12) >> 2); // mask = 1100; shift twice to the right
}

/**
 * Merges the x and y value into one result
 *
 * @brief Generates a request from coordinates
 * @details Shifts the x and y value accordingly and also generates the parity bit
 * @param x The x value
 * @param y The y value
 * @param request Pointer to the request data
 */
static void assemble_request(uint8_t x, uint8_t y, uint16_t *request) {
    // using schema: p000xxxxxxyyyyyy
    // bits 0 - 6
    uint16_t coord = (x << 6) | y;
    // bit 15
    uint8_t parity = calculate_parity(coord, 14);
    // assemble
    *request = (parity << 15) | coord;
}

/**
 * Shoots at the current location
 *
 * @brief Sends 1 byte of information to the server
 * @details encodes the current x and y location, calculates a parity bit and sends using the send function.
 * @param x The x value
 * @param y The y value
 */
static void shoot(uint8_t x, uint8_t y) {
    uint16_t request;
    assemble_request(x, y, &request);
    ssize_t size = send(sockfd, &request, sizeof(request), 0);
    if (size < 0) error_exit(strerror(errno), false);
}

/**
 * Handles the received hit value and sets x and y
 *
 * @brief Updates the map respectively and sets x and y
 * @details iterates through every square on the map
 * @param hit The received hit value
 * @param map Pointer to the map 2d array
 * @param x Pointer to the x value
 * @param y Pointer to the y value
 */
static void handle_result(uint8_t hit, uint8_t (*map)[MAP_SIZE], uint8_t *x, uint8_t *y) {
    switch (hit) {
        case 0:
            map[*x][*y] = SQUARE_EMPTY;
            break;
        case 1:
        case 2:
            map[*x][*y] = SQUARE_HIT;
            break;
        default:
            assert(0);
    }

    if (*y > MAP_SIZE - 1) {
        error_exit("End of map reached. Aborting.", false);
    }

    if (*x >= MAP_SIZE - 1) {
        *x = 0;
        *y += 1;
    } else *x += 1;
}

/**
 * Establishes the connection using the given host and port
 *
 * @brief Creates a socket and attempts to connect to the server
 * @details Sets global variables to allow convenient cleanup
 * @param host Pointer to the hostname
 * @param port Pointer to the port
 */
static void establish_connection(char **host, char **port) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    // Returns 0 or error code but does not set errno
    int res = getaddrinfo(*host, *port, &hints, &ai);
    if (res != 0) error_exit((char *) gai_strerror(res), false);

    sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
    if (sockfd < 0) error_exit(strerror(errno), false);

    res = connect(sockfd, ai->ai_addr, ai->ai_addrlen);
    if (res < 0) error_exit(strerror(errno), false);

    printf("[%s] Connection established.\n", pgm_name);
}

/**
 * Program entry point.
 * @brief This function takes care about parameters, creates and connects the socket and sends and receives
 * @details The main function performs argument parsing using the getopt function. Duplicate options without arguments
 * are not allowed. This implementation does not remember fields that were hit.
 * @param argc The argument counter.
 * @param argv The argument vector.
 * @return Returns EXIT_SUCCESS on success, 2 on parity error, 3 on invalid coordinates and 1 otherwise
 */
int main(int argc, char *argv[]) {
    // Parse arguments
    char *host = DEFAULT_HOST, *port = DEFAULT_PORT;
    parse_arguments(argc, argv, &host, &port);

    // Establish the connection
    ssize_t size;
    uint8_t buffer;
    establish_connection(&host, &port);

    int result = EXIT_SUCCESS;

    uint8_t x = 0, y = 0;
    uint8_t map[MAP_SIZE][MAP_SIZE];
    // Write SQUARE_UNKNOWN to 2d map array
    memset(map, SQUARE_UNKNOWN, sizeof(map[0][0]) * MAP_SIZE * MAP_SIZE);

    // Fire initial shot to start the communication
    shoot(x, y);

    bool game_over = false;

    while (!game_over && (size = recv(sockfd, &buffer, sizeof(buffer), 0)) > 0) {
        uint8_t hit, status;
        parse_response(buffer, &hit, &status);

        switch (status) {
            case 0:
                // Game is going on
                handle_result(hit, map, &x, &y);
                break;
            case 1:
                // Game over; check hit to determine if client won; return value was already set
                if (hit == 3) printf("[%s] I win :)\n", pgm_name);
                else printf("[%s] game lost\n", pgm_name);
                game_over = true;
                break;
            case 2:
                // Parity error
                fprintf(stderr, "[%s] parity error\n", pgm_name);
                game_over = true;
                result = status;
            case 3:
                // Invalid coordinate
                fprintf(stderr, "[%s] invalid coordinate\n", pgm_name);
                game_over = true;
                result = status;
            default:
                assert(0);
        }

        shoot(x, y);
    }

    // 0 >= orderly shutdown; -1 == error
    if (size < 0) error_exit(strerror(errno), false);

    print_map(map); // DEBUG ONLY

    clean_up();
    return result;
}