/**
 * @file server.c
 * @author Stefan Geyer <stefan.geyer@student.tuwien.ac.at>
 * @date 05.11.2017
 *
 * @brief Server program module.
 *
 * The server creates the ships and waits for clients to connect.
 * When the server receives a message, it will reply respectively.
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

// Static variables for things you might want to access from several functions:
static const char *port = DEFAULT_PORT; /**< the port to bind to */

// Static variables for resources that should be freed before exiting:
static struct addrinfo *ai = NULL;      /**< addrinfo struct */
static int sockfd = -1;                 /**< socket file descriptor */
static int connfd = -1;                 /**< connection file descriptor */

static int count_len2 = SHIP_CNT_LEN2;  /**< keeps track of the amount of ships with length 2 */
static int count_len3 = SHIP_CNT_LEN3;  /**< keeps track of the amount of ships with length 3 */
static int count_len4 = SHIP_CNT_LEN4;  /**< keeps track of the amount of ships with length 4 */

/**
 * @struct coord
 * @brief maps an x and y coordinate together
 * @details typedef defined as coord_t
 */
typedef struct coord {
    int x; /**< x coord */
    int y; /**< y coord */
} coord_t; /**< coord typedef */

/**
 * @struct ship
 * @brief Maps multiple coordinates together. Also keeps track of it's size and where it has been hit.
 * Every ship acts as a linked list entry and keeps the pointer to the next element.
 */
typedef struct ship {
    int size; /**< size of the ship */
    coord_t *coords; /**< coord list */
    bool *hits; /**< hit list */
    struct ship *next; /**< next element */
} ship_t; /**< ship typedef */

static char *pgm_name; /**< The program name */
static ship_t *head; /**< The head element of the ship list. (Linked list structure) */

/**
 * Mandatory usage function.
 * @brief This function writes helpful usage information about the program to stderr.
 * @details global variables: pgm_name
 */
static void usage(void) {
    fprintf(stderr, "SYNOPSIS\n\t%s [-p PORT] SHIP1...\n", pgm_name);
    exit(EXIT_FAILURE);
}

/**
 * Cleans up before the program exits
 * @brief Coses socket connections and frees the addrinfo and allocated memory
 * @details Frees sockfd, ai and ship structs
 */
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
 * Creates a ship with the given parameters
 *
 * @brief Based on the bow and stern, the other squares are calculated
 * @details If the ship is neither vertical nor horizontal, NULL is returned
 * Allocates memory for the ship using malloc. The ships act like a linked list data structure
 * and keep reference of the next sibling.
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

    ship = malloc(sizeof(ship_t));
    ship->coords = malloc(sizeof(coord_t) * size);
    ship->hits = malloc(sizeof(bool) * size);
    // check for errors
    if (ship == NULL || ship->coords == NULL || ship->hits == NULL) error_exit("Could not allocate memory.", false);

    ship->size = size;

    for (int i = low; i <= high; i++) {
        coord_t v = {bow->x, i}, h = {i, bow->y};
        ship->coords[i - low] = vertical ? v : h;
        ship->hits[i - low] = false;
    }

    return ship;
}

/**
 * Checks whether this ship can be added to the map or not. Must be called before adding the ship.
 *
 * @param ship
 */
static void validate_ship(ship_t *ship) {
    char error[50];

    // a ship's size must be between 2 and 4
    if (ship->size >= MIN_SHIP_LEN && ship->size <= MAX_SHIP_LEN) {
        // Check if too many ships with the given size exist
        if ((ship->size == 2 && count_len2-- > 0) || (ship->size == 3 && count_len3-- > 0) ||
            (ship->size == 4 && count_len4-- > 0)) {
            // Make sure the new ship does not overlap with any existing ships
            for (int i = 0; i < ship->size; i++) {
                coord_t nc = ship->coords[i];
                ship_t *curr = head;
                while (curr != NULL) {
                    for (int j = 0; j < curr->size; j++) {
                        coord_t ec = curr->coords[j];
                        if (nc.x == ec.x && nc.y == ec.y) {
                            sprintf(error, "There are overlapping ships at %c%d", 65 + nc.x, nc.y);
                            error_exit(error, true);
                        }
                    }
                    curr = curr->next;
                }
            }
        } else {
            sprintf(error, "There are too many ships with the length %d", ship->size);
            error_exit(error, true);
        }
    } else {
        sprintf(error, "Invalid ship length. Each ship must be between %d and %d squares long", MIN_SHIP_LEN,
                MAX_SHIP_LEN);
        error_exit(error, true);
    }

}

/**
 * Adds a ship to the linked list structure
 *
 * @brief Takes the parameter and appends it to the last element in the list.
 * If the head is NULL, the param will be the new head.
 * @details gobal varables: head
 * @param child The new element
 */
static void add_ship(ship_t *child) {
    // make sure the ship does not violate any rules
    validate_ship(child);

    if (head == NULL) {
        head = child;
    } else {
        ship_t *curr = head;
        while (curr->next != NULL) curr = curr->next;
        curr->next = child;
    }
}

/**
 * Checks whether the given ship is fully destroyed or not.
 *
 * @brief Checks the hits array on every index.
 * @param ship The ship to check
 * @return true if destroyed, false otherwise.
 */
static bool destroyed(ship_t *ship) {
    for (int i = 0; i < ship->size; i++)
        if (!ship->hits[i]) return false;
    return true;
}

/**
 * Checks whether all ships in the list are destroyed
 * @brief Iterates through the list to check. Also returns true, when the list is empty
 * @return true if all are destroyed, false otherwise.
 */
static bool all_destroyed(void) {
    ship_t *curr = head;
    while (curr != NULL) {
        if (!destroyed(curr)) return false;
        curr = curr->next;
    }
    return true;
}

/**
 * Checks if there is a ship at the given coord.
 *
 * @brief Checks if any element in the list contains the given coord.
 * @details Returns the respective status code.
 * @param coord The coord to check
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

/**
 * Takes argc and argv from the main function and sets required properties
 *
 * @brief Parses options and makes sure there are exactly 6 ships.
 * @details Parses port; can occur once at max.
 * @param argc arg count
 * @param argv arg vector
 */
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

    if (opt_p > 1) error_exit("Port option can only be set once.", true);

    // Check whether there are the correct amount of positional arguments
    // or an invalid option were supplied. (6 Ships)
    if ((argc - optind) != 6 || has_invalid_option) usage();

    for (int i = 0; i < (argc - optind); i++) {
        char *arg = argv[optind + i];
        char error[100];
        int len = (int) strlen(arg);
        if (len != 4) {
            sprintf(error, "wrong syntax for ship coordinates: %s", arg);
            error_exit(error, true);
        }
        // ASCII value for A is 65 and for 0 it is 48 --> generate an int between 0 and 9
        int bx = arg[0] - 65, by = arg[1] - 48, sx = arg[2] - 65, sy = arg[3] - 48;
        if (bx < 0 || bx > 9 || by < 0 || by > 9 || sx < 0 || sx > 9 || sy < 0 || sy > 9) {
            sprintf(error, "coordinates outside of map: %s", arg);
            error_exit(error, true);
        }

        coord_t bow = {bx, by}, stern = {sx, sy};
        ship_t *new = create_ship(&bow, &stern);
        if (new == NULL) {
            sprintf(error, "ships must be aligned either horizontally or vertically: %s", arg);
            error_exit(error, true);
        }
        add_ship(new);
    }
}

/**
 * Program entry point.
 * @brief This function takes care about parameters, creates the socket, waits for incoming connections and sends and receives
 * @details The main function performs argument parsing. Once a connection is established, the server sends and receives simultaneously.
 * @param argc The argument counter.
 * @param argv The argument vector.
 * @return Returns EXIT_SUCCESS on success, 2 on parity error, 3 on invalid coordinates and 1 otherwise
 */
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

    printf("[%s] Server is bound to port. Waiting for incoming connections...\n", pgm_name);

    connfd = accept(sockfd, NULL, NULL);
    if (connfd < 0) error_exit(strerror(errno), false);

    printf("[%s] Connection established.\n", pgm_name);

    int result = EXIT_SUCCESS;
    bool game_over = false;
    ssize_t recv_size, send_size;
    int rounds = 1;
    char buffer[80];
    memset(buffer, 0, sizeof(buffer));

    while (!game_over && (recv_size = recv(connfd, buffer, sizeof(buffer), 0)) > 0) {
        int hit = 0, status = 0;
        char b = buffer[0];
        // even parity --> 0
        if (calculate_parity(b, 7) == 0) {
            int c = b & 127; // mask = 0111 1111
            // int p = (in & 128) >> 7; // mask = 1000 0000; after shift 7 right = 1 / 0
            int x = c % 10, y = c / 10;

            if (x >= 0 && x <= 9 && y >= 0 && y <= 9) {
                coord_t attempt = {x, y};
                hit = shoot(&attempt);
                if (hit == 3) {
                    // result = default
                    status = 1;
                    game_over = true;
                    printf("[%s] client wins in %d rounds\n", pgm_name, rounds);
                } else if (rounds >= MAX_ROUNDS) {
                    // result = default
                    status = 1;
                    game_over = true;
                    printf("[%s] game lost\n", pgm_name);
                }
            } else {
                status = 3;
                result = status;
                game_over = true;
                fprintf(stderr, "[%s] invalid coordinate\n", pgm_name);
            }
        } else {
            status = 2;
            result = status;
            game_over = true;
            fprintf(stderr, "[%s] parity error\n", pgm_name);
        }

        char out = (char) ((status << 2) | hit);
        send_size = send(connfd, &out, sizeof(out), 0);
        if (send_size < 0) error_exit(strerror(errno), false);

        rounds++;
    }

    // 0 == orderly shutdown; -1 == error
    if (recv_size < 0) error_exit(strerror(errno), false);

    clean_up();
    return result;
}
