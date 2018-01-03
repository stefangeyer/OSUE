#ifndef UE3_COMMON_H
#define UE3_COMMON_H

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

#define SHM_NAME "/authshm"
#define PERMISSION (0600)

#define USERNAME_LENGTH 50
#define PASSWORD_LENGTH 50
#define MESSAGE_LENGTH 1024
#define CSV_DELIMITER ";"

// Request states
#define REQUEST_LOGIN = 0
#define REQUEST_REGISTER = 1
#define REQUEST_READ = 2
#define REQUEST_WRITE = 3
#define REQUEST_LOGOUT = 4

// Response state
#define RESPONSE_LOGIN_SUCCESS 5
#define RESPONSE_LOGIN_FAILURE 6
#define RESPONSE_REGISTER_SUCCESS 7
#define RESPONSE_REGISTER_FAILURE 8
#define RESPONSE_READ_SUCCESS 9
#define RESPONSE_READ_FAILURE 10
#define RESPONSE_WRITE_SUCCESS 11
#define RESPONSE_WRITE_FAILURE 12
#define RESPONSE_LOGOUT 13

typedef struct authshm {
    char *username;
    char *password;
    char *message;
    unsigned int state;
} authshm_t;

authshm_t *memory_create(void);
void memory_destroy(authshm_t *shared);

#endif //UE3_COMMON_H
