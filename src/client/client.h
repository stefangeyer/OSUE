#ifndef UE3_CLIENT_H
#define UE3_CLIENT_H

#include "../common/common.h"

#define CONV_CODE_SUCCESS (0)
#define CONV_CODE_FAILURE (-1)
#define CONV_CODE_INVALID_SESSION (-2)
#define CONV_CODE_INVALID_RESPONSE (-3)

void request_register(auth_memory_t *shared, char *username, char *password, char *session, char *secret);
int response_register(auth_memory_t *shared, char *username, char *password, char *session, char *secret);

void request_login(auth_memory_t *shared, char *username, char *password, char *session, char *secret);
int response_login(auth_memory_t *shared, char *username, char *password, char *session, char *secret);

void request_logout(auth_memory_t *shared, char *username, char *password, char *session, char *secret);
int response_logout(auth_memory_t *shared, char *username, char *password, char *session, char *secret);

void request_read(auth_memory_t *shared, char *username, char *password, char *session, char *secret);
int response_read(auth_memory_t *shared, char *username, char *password, char *session, char *secret);

void request_write(auth_memory_t *shared, char *username, char *password, char *session, char *secret);
int response_write(auth_memory_t *shared, char *username, char *password, char *session, char *secret);

#endif //UE3_CLIENT_H
