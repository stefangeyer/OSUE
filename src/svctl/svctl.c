#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include <assert.h>
#include <string.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <limits.h>
#include <errno.h>
#include "svctl.h"

char *pgm_name, key[KEY_SIZE];
int id, size, command;

void usage(void) {
    fprintf(stderr, "USAGE: ./%s [-c <size>|-k|-e|-d] <secvault id>\n", pgm_name);
    exit(EXIT_FAILURE);
}

void error_exit(char *message) {
    fprintf(stderr, "%s: %s\n", pgm_name, message);
    exit(EXIT_FAILURE);
}

void read_key(char *buffer, int size) {
    for (int i = 0; i < size; i++) {
        printf("Please enter a character: ");
        int c = fgetc(stdin);
        if (c == EOF) {
            error_exit("Cannot read from stdin");
        }
        buffer[i] = (char) c;
    }
}

int parse_int(char *str) {
    char *endptr;
    long res = strtol(str, &endptr, 10);

    if (endptr == str) {
        // nothing parsed from the string, handle errors
        error_exit("Nothing could be parsed from string");
    }
    if ((res == LONG_MAX || res == LONG_MIN) && errno == ERANGE) {
        // out of range, handle
        error_exit("Parsed string was out of range");
    }

    return (int) res;
}

void parse_arguments(int argc, char *argv[]) {
    pgm_name = argv[0];

    int c, opt_c = 0, opt_ked = 0;
    bool invopt = false;

    // 3rd param: optstring defines legitimate option characters.
    // Appending : to an argument character implies, that the option requires an argument
    while ((c = getopt(argc, argv, "c:ked")) != -1) {
        switch (c) {
            case 'c':
                size = parse_int(optarg);
                if (size < 1 || size > 1048576) usage();

                command = c;
                opt_c++;
                break;
            case 'k':
            case 'e':
            case 'd':
                command = c;
                opt_ked++;
                break;
            case '?':
                invopt = true;
                break;
            default:
                assert(0);
        }
    }

    // Check whether there are the correct amount of positional arguments and if the options where provided correctly
    if ((argc - optind) != 1 || invopt || (opt_c + opt_ked) > 1) usage();

    id = parse_int(argv[optind]);

    if (command == 'c' || command == 'k') {
        read_key(key, KEY_SIZE);
    }
}

int main(int argc, char *argv[]) {
    memset(key, 0, KEY_SIZE);
    parse_arguments(argc, argv);

    sv_message_t message;
    memset(&message, 0, sizeof(message));
    message.id = id;
    message.size = size;
    memcpy(message.key, key, KEY_SIZE);

    int ctlfd = open("/dev/sv_ctl", O_RDWR);
    if (ctlfd < 0) {
        error_exit("Cannot open secvault device");
    }

    if (ioctl(ctlfd, command, &message) < 0) {
        error_exit("Command execution failed");
    }

    printf("Command was executed successfully\n");

    return EXIT_SUCCESS;
}