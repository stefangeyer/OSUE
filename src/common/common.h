#ifndef UE3_COMMON_H
#define UE3_COMMON_H

#include <semaphore.h>
#include <stdbool.h>

#define SHM_NAME "/authshm"
#define PERMISSION (0600)

#define USERNAME_LENGTH (50)
#define PASSWORD_LENGTH (50)
#define SECRET_LENGTH (1024)
#define SESSION_LENGTH (10)

// semaphores
#define SEM_SERVER "/auth_server"
#define SEM_CLIENT "/auth_client"
#define SEM_MUTEX "/auth_mutex"

// Request states
#define REQUEST_LOGIN (1)
#define REQUEST_REGISTER (2)
#define REQUEST_READ (3)
#define REQUEST_WRITE (4)
#define REQUEST_LOGOUT (5)

// Response state
#define RESPONSE_LOGIN_SUCCESS (-1)
#define RESPONSE_LOGIN_FAILURE (-2)
#define RESPONSE_REGISTER_SUCCESS (-3)
#define RESPONSE_REGISTER_FAILURE (-4)
#define RESPONSE_READ_SUCCESS (-5)
#define RESPONSE_READ_FAILURE (-6)
#define RESPONSE_WRITE_SUCCESS (-7)
#define RESPONSE_WRITE_FAILURE (-8)
#define RESPONSE_LOGOUT_SUCCESS (-9)
#define RESPONSE_LOGOUT_FAILURE (-10)
#define RESPONSE_INVALID_SESSION (-11)

typedef struct auth_memory {
    char username[USERNAME_LENGTH];
    char password[PASSWORD_LENGTH];
    char secret[SECRET_LENGTH];
    char session[SESSION_LENGTH];
    int state;
} auth_memory_t;

typedef struct auth_semaphores {
    sem_t *mutex;
    sem_t *server;
    sem_t *client;
} auth_semaphores_t;

/**
 * Creates and maps the shared memory used in this program
 *
 * @brief Creates shared memory at SHM_NAME and with permissions PERMISSION
 * @details This function will generate an error if the shared memory was already created
 * @return The creates shared memory
 */
auth_memory_t *memory_create(void);

/**
 * Unlinks and destroys the shared memory used in this program
 *
 * @brief Unmaps and destroys the created memory instance
 * @param shared The memory to destroy
 */
void memory_destroy(auth_memory_t *shared);

/**
 * Opens existing shared memory instance
 *
 * @brief Opens existing shared memory. The memory must have been created before of this function will error
 * @return The opened memory
 */
auth_memory_t *memory_open(void);

/**
 * Closes an open shared memory instance
 *
 * @brief Closes an open shared memory instance but does not destroy it
 * @param shared The memory to close
 */
void memory_close(auth_memory_t *shared);

/**
 * Creates an instance of auth_semaphores_t containing the required semaphores
 *
 * @brief Creates an instance of auth_semaphores_t and sets up the semaphore links for server, client and mutex
 * @details This function will error if the semaphores were already created
 * @return A pointer to the allocated structure
 */
auth_semaphores_t *semaphores_create(void);

/**
 * Destroys the used semaphores
 *
 * @brief Unlinks and destroys the semaphores, then frees the container
 * @param semaphores The semaphores to destroy
 */
void semaphores_destroy(auth_semaphores_t *semaphores);

/**
 * Opens the existing semaphores and maps them together
 *
 * @brief The semaphores to open must have been created before
 * @return The opened semaphores
 */
auth_semaphores_t *semaphores_open(void);

/**
 * Closes the opened semaphores
 *
 * @brief Closes the opened semaphores, but does not destroy them
 * @param semaphores The semaphores to close
 */
void semaphores_close(auth_semaphores_t *semaphores);

/**
 * @brief Prints the given reason to stderr and exits with EXIT_FAILURE
 * @param reason Reason to print
 */
void error_exit(char *reason);

#endif //UE3_COMMON_H
