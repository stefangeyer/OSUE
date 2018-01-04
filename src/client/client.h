#ifndef UE3_CLIENT_H
#define UE3_CLIENT_H

#include "../common/common.h"

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
int logout(char *session);

/**
 *
 * @param session
 * @param secret
 * @return
 */
int read_secret(char *session, char *secret);

/**
 *
 * @param session
 * @param secret
 * @return
 */
int write_secret(char *session, char *secret);

#endif //UE3_CLIENT_H
