#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "main.h"
#include "process.h"

static char *pgm_name;

int main(int argc, char *argv[]) {
    parse_arguments(argc, argv);

    long int amount;
    read_line_amount(&amount);

    char lines[amount][LINE_LENGTH];

    read_lines(&amount, lines);

    forksort(amount, lines, pgm_name);

    return EXIT_SUCCESS;
}

void usage(void) {
    error_exit("SYNOPSIS:\n\t%s [-i ignorepraefix] <directory>");
}

void clean_up() {

}

void error_exit(char *message) {
    if (message != NULL && strlen(message) > 0) fprintf(stderr, "%s: %s\n", pgm_name, message);
    clean_up();
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

        read_line(&line);
        strcpy(lines[i], line);

        free(line);
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

    // Remove newline
    (*lineptr)[strcspn(*lineptr, "\n")] = 0;
}