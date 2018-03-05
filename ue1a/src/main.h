#ifndef UE1A_MAIN_H
#define UE1A_MAIN_H

#define LINE_LENGTH 100

static void process_input(char *file_name, int *read, int *written);
static void clean_up();
void usage(void);
void error_exit(char *reason);

#endif //UE1A_MAIN_H
