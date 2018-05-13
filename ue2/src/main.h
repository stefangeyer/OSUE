#ifndef UE2_MAIN_H
#define UE2_MAIN_H

#define LINE_LENGTH (62)

void usage(void);

void clean_up();

void error_exit(char *message);

void parse_arguments(int argc, char *argv[]);

void read_line(char **lineptr);

void read_line_amount(long int *amount);

void read_lines(const long int *amount, char lines[][LINE_LENGTH]);

#endif //UE2_MAIN_H
