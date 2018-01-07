#ifndef UE3_CLIENT_H
#define UE3_CLIENT_H

#include "../common/common.h"

void client_init(void);

/**
 *
 * @param username
 * @param password
 * @return
 */
int signup(char *username, char *password);

/**
 *
 * @param username
 * @param password
 */
int login(char *username, char *password, char *session);

/**
 *
 * @param session
 * @return
 */
int logout(char *username, char *session);

/**
 *
 * @param session
 * @param secret
 * @return
 */
int read_secret(char *username, char *session, char *secret);

/**
 *
 * @param session
 * @param secret
 * @return
 */
int write_secret(char *username, char *session, char *secret);

void client_destroy(void);

#endif //UE3_CLIENT_H
