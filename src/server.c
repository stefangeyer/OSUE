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

typedef struct coord {
    int x;
    int y;
} coord_t;

typedef struct ship {
    int size;
    coord_t *coords;
    bool *hits;
    struct ship *next;
} ship_t;

static char *pgm_name; /**< The program name */
static ship_t *head; /**< The head element of the ship list. (Linked list structure) */



/**
 * Mandatory usage function.
 * @brief This function writes helpful usage information about the program to stderr.
 * @details global variables: pgm_name
 */
static void usage(void) {
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
static void error_exit(char *message, bool show_usage) {
    fprintf(stderr, "%s: An error has occurred: %s\n", pgm_name, message);
    if (show_usage) usage();
    else exit(EXIT_FAILURE);
}

/**
 *
 * @param bow Bow's coordinates (Bug)
 * @param stern Stern's coordinates (Heck)
 * @return The ship if valid coordinates were provided, NULL otherwise (e.g. for diagonal ships)
 */
static ship_t *create_ship(coord_t *bow, coord_t *stern) {
    ship_t *ship;
    int size, low, high;
    bool vertical;

    if (bow->x == stern->x) {
        // Ship is aligned vertically
        size = 1 + abs(bow->y - stern->y);
        vertical = true;
        low = min(bow->y, stern->y);
        high = max(bow->y, stern->y);
    } else if (bow->y == stern->y) {
        // Ship is aligned horizontally
        size = 1 + abs(bow->x - stern->x);
        vertical = false;
        low = min(bow->x, stern->x);
        high = max(bow->x, stern->x);
    } else {
        // Invalid coordinates
        return NULL;
    }

    // TODO
    // check for correct amount of ships with correct lengths + there must be 1 empty space between the ships
    // a ship's size must be between 2 and 4

    ship = malloc(sizeof(ship_t));
    ship->coords = malloc(sizeof(coord_t) * size);
    ship->hits = malloc(sizeof(bool) * size);
    // check for errors
    if (ship == NULL) error_exit("Could not allocate memory.", false);

    ship->size = size;

    for (int i = low; i <= high; i++) {
        coord_t c1 = {bow->x, i};
        coord_t c2 = {i, bow->y};
        ship->coords[i - low] = vertical ? c1 : c2;
        ship->hits[i - low] = false;
    }

    return ship;
}

static void add_ship(ship_t *child) {
    if (head == NULL) {
        head = child;
    } else {
        ship_t *curr = head;
        while (curr->next != NULL) curr = curr->next;
        curr->next = child;
    }

}

static bool destroyed(ship_t *ship) {
    for (int i = 0; i < ship->size; i++)
        if (!ship->hits[i]) return false;
    return true;
}

static bool all_destroyed() {
    ship_t *curr = head;
    while (curr != NULL) {
        if (!destroyed(curr)) return false;
        curr = curr->next;
    }
    return true;
}

/**
 *
 * @param coord
 * @return 0 = no hit
 *         1 = hit, but not destroyed
 *         2 = hit and destroyed, but there are other ships left
 *         3 = hit and destroyed and this was the last ship --> client wins
 */
static int shoot(coord_t *coord) {
    ship_t *curr = head;
    int result = 0;
    while (curr != NULL) {
        for (int i = 0; i < curr->size; i++) {
            if (curr->coords[i].x == coord->x && curr->coords[i].y == coord->y) {
                curr->hits[i] = true;
                result = destroyed(curr) ? 2 : 1;
                break;
            }
        }
        curr = curr->next;
    }
    // Always need to check if the game has ended.
    if (all_destroyed()) result = 3;
    return result;
}

static void parse_arguments(int argc, char **argv) {
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

    if (opt_p > 1) error_exit("Port option can be set only once.", true);

    // Check whether there are the correct amount of positional arguments
    // or an in valid option were supplied. (6 Ships)
    if ((argc - optind) != 6 || has_invalid_option) usage();

    for (int i = 0; i < (argc - optind); i++) {
        char *arg = argv[optind + i];
        int len = (int) strlen(arg);
        if (len != 4) error_exit("Each argument must consist of two capital letters and two digits.", true);
        // ASCII value for A is 65 and for 0 it is 48 --> generate an int between 0 and 9
        int bx = arg[0] - 65, by = arg[1] - 48, sx = arg[2] - 65, sy = arg[3] - 48;
        if (bx < 0 || bx > 9 || by < 0 || by > 9 || sx < 0 || sx > 9 || sy < 0 || sy > 9)
            error_exit("The coordinates of each ship must be between A and J (x) and 0 and 9 (y).", true);
        coord_t bow = {by, by}, stern = {sx, sy};
        add_ship(create_ship(&bow, &stern));
    }
}

static void clean_up(void) {
    ship_t *curr;
    while ((curr = head) != NULL) {
        head = head->next;
        // Free allocated memory
        free(curr->coords);
        free(curr->hits);
        free(curr);
    }

    close(connfd);
    close(sockfd);
    freeaddrinfo(ai);
}

static void process_response(char c) {

}

int main(int argc, char *argv[]) {
    // This also handles setting pgm_name
    parse_arguments(argc, argv);

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    // Returns 0 or error code but does not set errno
    int res = getaddrinfo(NULL, port, &hints, &ai);
    if (res != 0) error_exit((char *) gai_strerror(res), false);

    sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
    if (sockfd < 0) error_exit(strerror(errno), false);

    /* Useful to avoid the error "Address already in use"
     * (EADDRINUSE) with bind upon restarting your server program
     * (otherwise the port remains unusable for approximately
     * 1 min after the server was terminated)
     */
    int val = 1;
    res = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof val);
    if (res) error_exit(strerror(errno), false);

    res = bind(sockfd, ai->ai_addr, ai->ai_addrlen);
    if (res) error_exit(strerror(errno), false);

    res = listen(sockfd, 1);
    if (res) error_exit(strerror(errno), false);

    connfd = accept(sockfd, NULL, NULL);
    if (connfd) error_exit(strerror(errno), false);

    /* TODO
     * Here you might want to add variables to keep track of the game status,
     * for instance the number of rounds that have been played.
     */
    int rounds = 1;
    char in;
    ssize_t recv_size;

    while (rounds <= MAX_ROUNDS) {
        /* TODO
         * add code to:
         *  - wait for a request from the client
         *  - check for a parity error or invalid coordinates in the request
         *  - check whether a ship was hit and determine the status to return
         *  - send an according response to the client
         */
        recv_size = recv(connfd, &in, 1, MSG_WAITALL);
        if (recv_size < 0) error_exit(strerror(errno), false);

        int c = in & 0x07, x = c % 10, y = x / 10, p = in & 0x08, hit = 0, status = 0;

        if (calculate_parity(c) == p) {
            if (x >= 0 && x <= 9 && y >= 0 && y <= 9) {
                coord_t attempt = {x, y};
                hit = shoot(&attempt);
                if (hit == 3 || rounds == MAX_ROUNDS) status = 1;
            } else {
                status = 3;
            }
        } else {
            status = 2;
        }

        char out = (char) ((status << 2) | hit);
        send(connfd, &out, 1, MSG_WAITALL);

        rounds++;
    }

    /* TODO
     * cleanup
     */
    clean_up();
}
