/**
 * @file common.h
 * @author Stefan Geyer <stefan.geyer@student.tuwien.ac.at>
 * @date 08.01.2017
 *
 * @brief Common program module header.
 *
 * Defines a bunch of constants and the structures for semaphores and shared memory.
 * Also defines functions that either set up or interact with shm and sem.
 **/


#ifndef UE3_COMMON_H
#define UE3_COMMON_H

#include <semaphore.h>
#include <stdbool.h>

#define SHM_NAME "/authshm" /**< Name of the shared memory **/
#define PERMISSION (0600) /**< Create permissions for shared memory and semaphores **/

#define USERNAME_LENGTH (50) /**< Max username length **/
#define PASSWORD_LENGTH (50) /**< Max password length **/
#define SECRET_LENGTH (1024) /**< Max secret length **/
#define SESSION_LENGTH (50) /**< Max session length **/

// semaphores
#define SEM_SERVER "/auth_server" /**< Name of server semaphore */
#define SEM_CLIENT "/auth_client" /**< Name of client semaphore */
#define SEM_MUTEX "/auth_mutex" /**< Name of mutex semaphore */

// Request states
#define REQUEST_LOGIN (1) /**< Request state for login */
#define REQUEST_REGISTER (2) /**< Request state for register */
#define REQUEST_READ (3) /**< Request state for read */
#define REQUEST_WRITE (4) /**< Request state for write */
#define REQUEST_LOGOUT (5) /**< Request state for logout */

// Response state
#define RESPONSE_LOGIN_SUCCESS (-1) /**< response state for login success */
#define RESPONSE_LOGIN_FAILURE (-2) /**< response state for login failure */
#define RESPONSE_REGISTER_SUCCESS (-3) /**< response state for register success */
#define RESPONSE_REGISTER_FAILURE (-4) /**< response state for register failure */
#define RESPONSE_READ_SUCCESS (-5) /**< response state for read success */
#define RESPONSE_READ_FAILURE (-6) /**< response state for read failure */
#define RESPONSE_WRITE_SUCCESS (-7) /**< response state for write success */
#define RESPONSE_WRITE_FAILURE (-8) /**< response state for write failure */
#define RESPONSE_LOGOUT_SUCCESS (-9) /**< response state for logout success */
#define RESPONSE_LOGOUT_FAILURE (-10) /**< response state for logout failure */
#define RESPONSE_INVALID_SESSION (-11) /**< response state for invalid session */

// error messages
#define ERROR_INTERRUPTED "Program interrupted. Aborting.\n"

/**
 * @struct Shared memory struct
 * @brief Struct that contains all data that is communicated between client and server
 * @details Typedef as auth_memory_t
 */
typedef struct auth_memory {
    bool server_available; /**< False, if server is unavailable */
    char username[USERNAME_LENGTH]; /**< username, if required */
    char password[PASSWORD_LENGTH]; /**< password, if required */
    char secret[SECRET_LENGTH]; /**< secret, if required */
    char session[SESSION_LENGTH]; /**< session, if required */
    int state; /**< current state */
} auth_memory_t;

/**
 * @struct Semaphore struct
 * @brief Strcut that maps the used semaphores together
 * @brief Typedef as auth_semaphore_t
 */
typedef struct auth_semaphores {
    sem_t *mutex; /**< mutex semaphore */
    sem_t *server; /**< server semaphore */
    sem_t *client; /**< client semaphore */
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
 * @brief DEBUG: Prints the content of the given shared memory object
 * @param shared The memory to print
 */
void print_memory(auth_memory_t *shared);

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
