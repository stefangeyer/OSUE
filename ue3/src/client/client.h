/**
 * @file client.h
 * @author Stefan Geyer <stefan.geyer@student.tuwien.ac.at>
 * @date 08.01.2017
 *
 * @brief Client program module header.
 *
 * Defines request and response functions for the client server handshake
 **/

#ifndef UE3_CLIENT_H
#define UE3_CLIENT_H

#include "../common/common.h"

#define CONV_CODE_SUCCESS (0) /**< Conversation was successful **/
#define CONV_CODE_FAILURE (-1) /**< Conversation failed (e.g. invalid credentials, user already exists) **/
#define CONV_CODE_INVALID_SESSION (-2) /**< Conversation failed due to invalid session **/
#define CONV_CODE_INVALID_RESPONSE (-3) /**< Conversation failed due to invalid response code **/

/**
 * @brief Writes the register request to the shared memory
 *
 * @param shared Shared memory
 * @param username Client's username
 * @param password Client's password
 * @param session Current session
 * @param secret Client's secret
 */
void request_register(auth_memory_t *shared, char *username, char *password, char *session, char *secret);

/**
 * @brief Reads the register response from the shared memory
 *
 * @param shared Shared memory
 * @param username Client's username
 * @param password Client's password
 * @param session Current session
 * @param secret Client's secret
 */
int response_register(auth_memory_t *shared, char *username, char *password, char *session, char *secret);

/**
 * @brief Writes the login request to the shared memory
 *
 * @param shared Shared memory
 * @param username Client's username
 * @param password Client's password
 * @param session Current session
 * @param secret Client's secret
 */
void request_login(auth_memory_t *shared, char *username, char *password, char *session, char *secret);

/**
 * @brief Reads the login response from the shared memory
 *
 * @param shared Shared memory
 * @param username Client's username
 * @param password Client's password
 * @param session Current session
 * @param secret Client's secret
 */
int response_login(auth_memory_t *shared, char *username, char *password, char *session, char *secret);

/**
 * @brief Writes the logout request to the shared memory
 *
 * @param shared Shared memory
 * @param username Client's username
 * @param password Client's password
 * @param session Current session
 * @param secret Client's secret
 */
void request_logout(auth_memory_t *shared, char *username, char *password, char *session, char *secret);

/**
 * @brief Reads the logout response from the shared memory
 *
 * @param shared Shared memory
 * @param username Client's username
 * @param password Client's password
 * @param session Current session
 * @param secret Client's secret
 */
int response_logout(auth_memory_t *shared, char *username, char *password, char *session, char *secret);

/**
 * @brief Writes the read request to the shared memory
 *
 * @param shared Shared memory
 * @param username Client's username
 * @param password Client's password
 * @param session Current session
 * @param secret Client's secret
 */
void request_read(auth_memory_t *shared, char *username, char *password, char *session, char *secret);

/**
 * @brief Reads the read response from the shared memory
 *
 * @param shared Shared memory
 * @param username Client's username
 * @param password Client's password
 * @param session Current session
 * @param secret Client's secret
 */
int response_read(auth_memory_t *shared, char *username, char *password, char *session, char *secret);

/**
 * @brief Writes the write request to the shared memory
 *
 * @param shared Shared memory
 * @param username Client's username
 * @param password Client's password
 * @param session Current session
 * @param secret Client's secret
 */
void request_write(auth_memory_t *shared, char *username, char *password, char *session, char *secret);

/**
 * @brief Reads the write response from the shared memory
 *
 * @param shared Shared memory
 * @param username Client's username
 * @param password Client's password
 * @param session Current session
 * @param secret Client's secret
 */
int response_write(auth_memory_t *shared, char *username, char *password, char *session, char *secret);

#endif //UE3_CLIENT_H
