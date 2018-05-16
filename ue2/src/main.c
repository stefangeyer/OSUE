/**
 * @file main.c
 * @author Stefan Geyer <e1625718 at student.tuwien.ac.at>
 * @date 16.05.2018
 *
 * @brief Main program module.
 *
 * Reads lines from the console and sorts them using a mergesort variant.
 **/

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <stdbool.h>
#include <zconf.h>
#include "main.h"
#include "process.h"

char* pgm_name;

/**
 * Parses arguments from argc and argv
 * @brief Converts arguments to usable variables
 * @details no global variables
 *
 * @param argc argument count
 * @param argv argument vector
 */
static void parse_arguments(int argc, char *argv[]);

/**
 * Reads a number from stdin
 *
 * @brief Using the read_line function a number is read and converted
 * @details The result is stored to the parameter.
 *
 * @param amount The read number
 */
static void read_line_amount(long int *amount);

/**
 * Read multiple lines from stdin
 *
 * @brief Reads amount lines and stores them into lines
 * @details Using the read_line function
 *
 * @param amount The amount of lines to read
 * @param lines The array to save the results to
 */
static void read_lines(const long int *amount, char lines[][LINE_LENGTH]);

/**
 * Program entry point.
 *
 * @brief This function takes care about parameters, and calls the forksort algorithm
 * @details Also performs argument parsing
 * @param argc The argument counter.
 * @param argv The argument vector.
 * @return Returns EXIT_SUCCESS or exits via a called function
 */
int main(int argc, char *argv[]) {
    parse_arguments(argc, argv);

    long int amount;
    read_line_amount(&amount);

    char lines[amount][LINE_LENGTH];

    read_lines(&amount, lines);

    forksort(amount, lines);

    return EXIT_SUCCESS;
}

void usage(void) {
    fprintf(stderr, "SYNOPSIS:\n\t%s\n", pgm_name);
    error_exit(NULL);
}

void error_exit(char *message) {
    if (message != NULL && strlen(message) > 0) fprintf(stderr, "%s: %s\n", pgm_name, message);
    exit(EXIT_FAILURE);
}

void parse_arguments(int argc, char *argv[]) {
    pgm_name = argv[0];

    if (argc > 1) usage();
}

void read_line_amount(long int *amount) {
    char *line;
    char *ptr;

    read_line(&line);
    line[strcspn(line, "\n")] = 0;

    *amount = strtol(line, &ptr, 10);

    if (*ptr != '\0') {
        error_exit("Provided line amount is not a number");
    }

    if (*amount < 1) {
        error_exit("Line amount must be positive");
    }

    free(line);
}

void read_lines(const long int *amount, char lines[][LINE_LENGTH]) {
    for (long int i = 0; i < *amount; i++) {
        char *line;
        bool empty = false;

        read_line(&line);

        if (line[0] == '\n') empty = true;

        strcpy(lines[i], line);

        free(line);

        if (empty) {
            error_exit("Empty line was provided");
        }
    }
}

void read_line(char **lineptr) {
    size_t len;

    *lineptr = NULL;

    if (getline(lineptr, &len, stdin) == -1) {
        error_exit("Cannot read line");
    }

    if (strlen(*lineptr) == 0) {
        error_exit("Read empty line");
    }
}