#include <string.h>
#include <stdio.h>
#include "client.h"

void request_register(auth_memory_t *shared, char *username, char *password, char *session, char *secret) {
    shared->state = REQUEST_REGISTER;
    strcpy(shared->username, username);
    strcpy(shared->password, password);
}

int response_register(auth_memory_t *shared, char *username, char *password, char *session, char *secret) {
    switch (shared->state) {
        case RESPONSE_REGISTER_SUCCESS:
            return CONV_CODE_SUCCESS;
        case RESPONSE_REGISTER_FAILURE:
            return CONV_CODE_FAILURE;
        default:
            printf("Code: %d", shared->state);
            return CONV_CODE_INVALID_RESPONSE;
    }
}

void request_login(auth_memory_t *shared, char *username, char *password, char *session, char *secret) {
    shared->state = REQUEST_LOGIN;
    strcpy(shared->username, username);
    strcpy(shared->password, password);
}

int response_login(auth_memory_t *shared, char *username, char *password, char *session, char *secret) {
    switch (shared->state) {
        case RESPONSE_LOGIN_SUCCESS:
            strcpy(session, shared->session);
            return CONV_CODE_SUCCESS;
        case RESPONSE_LOGIN_FAILURE:
            return CONV_CODE_FAILURE;
        default:
            return CONV_CODE_INVALID_RESPONSE;
    }
}

void request_logout(auth_memory_t *shared, char *username, char *password, char *session, char *secret) {
    shared->state = REQUEST_LOGOUT;
    strcpy(shared->username, username);
    strcpy(shared->session, session);
}

int response_logout(auth_memory_t *shared, char *username, char *password, char *session, char *secret) {
    switch (shared->state) {
        case RESPONSE_LOGOUT_SUCCESS:
            return CONV_CODE_SUCCESS;
        case RESPONSE_LOGIN_FAILURE:
            return CONV_CODE_FAILURE;
        case RESPONSE_INVALID_SESSION:
            return CONV_CODE_INVALID_SESSION;
        default:
            return CONV_CODE_INVALID_RESPONSE;
    }
}

void request_read(auth_memory_t *shared, char *username, char *password, char *session, char *secret) {
    shared->state = REQUEST_READ;
    strcpy(shared->username, username);
    strcpy(shared->session, session);
}

int response_read(auth_memory_t *shared, char *username, char *password, char *session, char *secret) {
    switch (shared->state) {
        case RESPONSE_READ_SUCCESS:
            strcpy(secret, shared->secret);
            return CONV_CODE_SUCCESS;
        case RESPONSE_READ_FAILURE:
            return CONV_CODE_FAILURE;
        case RESPONSE_INVALID_SESSION:
            return CONV_CODE_INVALID_SESSION;
        default:
            return CONV_CODE_INVALID_RESPONSE;
    }
}

void request_write(auth_memory_t *shared, char *username, char *password, char *session, char *secret) {
    shared->state = REQUEST_WRITE;
    strcpy(shared->username, username);
    strcpy(shared->session, session);
    strcpy(shared->secret, secret);
}

int response_write(auth_memory_t *shared, char *username, char *password, char *session, char *secret) {
    switch (shared->state) {
        case RESPONSE_WRITE_SUCCESS:
            strcpy(shared->secret, secret);
            return CONV_CODE_SUCCESS;
        case RESPONSE_WRITE_FAILURE:
            return CONV_CODE_FAILURE;
        case RESPONSE_INVALID_SESSION:
            return CONV_CODE_INVALID_SESSION;
        default:
            return CONV_CODE_INVALID_RESPONSE;
    }
}