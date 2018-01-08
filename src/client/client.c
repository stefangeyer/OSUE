#include <string.h>
#include "client.h"

static auth_memory_t *shared;
static auth_semaphores_t *semaphores;

void client_init(void) {
    shared = memory_open();
    semaphores = semaphores_open();
}

int signup(char *username, char *password) {
    if (!shared->server_available) error_exit("Server is not available");

    sem_wait(semaphores->mutex); // notify server, that the client wants to interact

    // prepare request
    shared->state = REQUEST_REGISTER;
    strcpy(username, shared->username);
    strcpy(password, shared->password);

    sem_post(semaphores->server); // request done

    sem_wait(semaphores->client); // wait for response

    // handle response
    switch (shared->state) {
        case RESPONSE_REGISTER_SUCCESS:
            return 0;
        case RESPONSE_REGISTER_FAILURE:
            return -1;
        default:
            error_exit("Received invalid response code");
            break;
    }

    return -1;
}

int login(char *username, char *password, char *session) {
    if (!shared->server_available) error_exit("Server is not available");

    sem_wait(semaphores->mutex); // notify server, that the client wants to interact

    // prepare request
    shared->state = REQUEST_LOGIN;
    strcpy(username, shared->username);
    strcpy(password, shared->password);

    sem_post(semaphores->server); // request done

    sem_wait(semaphores->client); // wait for response
    // handle response
    switch (shared->state) {
        case RESPONSE_LOGIN_SUCCESS:
            strcpy(session, shared->session);
            return 0;
        case RESPONSE_LOGIN_FAILURE:
            return -1;
        default:
            error_exit("Received invalid response code");
            break;
    }

    return 0;
}

int logout(char *username, char *session) {
    if (!shared->server_available) error_exit("Server is not available");

    sem_wait(semaphores->mutex); // notify server, that the client wants to interact

    // prepare request
    shared->state = REQUEST_LOGOUT;
    strcpy(username, shared->username);
    strcpy(session, shared->session);

    sem_post(semaphores->server); // request done

    sem_wait(semaphores->client); // wait for response
    // handle response
    switch (shared->state) {
        case RESPONSE_LOGOUT_SUCCESS:
            return 0;
        case RESPONSE_LOGIN_FAILURE:
            return -1;
        case RESPONSE_INVALID_SESSION:
            return -2;
        default:
            error_exit("Received invalid response code");
            break;
    }

    return -1;
}

int read_secret(char *username, char *session, char *secret) {
    if (!shared->server_available) error_exit("Server is not available");

    sem_wait(semaphores->mutex); // notify server, that the client wants to interact

    // prepare request
    shared->state = REQUEST_READ;
    strcpy(username, shared->username);
    strcpy(session, shared->session);

    sem_post(semaphores->server); // request done

    sem_wait(semaphores->client); // wait for response
    // handle response
    switch (shared->state) {
        case RESPONSE_READ_SUCCESS:
            strcpy(shared->secret, secret);
            return 0;
        case RESPONSE_READ_FAILURE:
            return -1;
        case RESPONSE_INVALID_SESSION:
            return -2;
        default:
            error_exit("Received invalid response code");
            break;
    }

    return -1;
}

int write_secret(char *username, char *session, char *secret) {
    if (!shared->server_available) error_exit("Server is not available");

    sem_wait(semaphores->mutex); // notify server, that the client wants to interact

    // prepare request
    shared->state = REQUEST_WRITE;
    strcpy(username, shared->username);
    strcpy(session, shared->session);
    strcpy(secret, shared->secret);

    sem_post(semaphores->server); // request done

    sem_wait(semaphores->client); // wait for response
    // handle response
    switch (shared->state) {
        case RESPONSE_WRITE_SUCCESS:
            strcpy(shared->secret, secret);
            return 0;
        case RESPONSE_WRITE_FAILURE:
            return -1;
        case RESPONSE_INVALID_SESSION:
            return -2;
        default:
            error_exit("Received invalid response code");
            break;
    }

    return -1;
}

void client_destroy(void) {
    if (!shared->server_available) error_exit("Server is not available");

    memory_close(shared);
    semaphores_close(semaphores);
}