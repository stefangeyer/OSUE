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

#define SHM_NAME "/osue_3crl_shm" /**< Name of the shared memory **/
#define PERMISSION (0600) /**< Create permissions for shared memory and semaphores **/

// semaphores
#define SEM_FREE "/osue_3clr_free" /**< Name of write semaphore */
#define SEM_USED "/osue_3clr_used" /**< Name of read semaphore */
#define SEM_MUTEX "/osue_3clr_mutex" /**< Name of mutex semaphore */

#define SOLUTION_MAX_EDGES (8) /**< Maximal number of edges in a solution */

/**
 * @struct Shared memory struct
 * @brief Struct that contains all data that is communicated
 * @details Typedef as circular_buffer_t
 */
typedef struct circular_buffer {

} circular_buffer_t;

/**
 * @struct Semaphore struct
 * @brief Strcut that maps the used semaphores together
 * @brief Typedef as auth_semaphore_t
 */
typedef struct semaphores {
    sem_t *mutex; /**< mutex semaphore */
    sem_t *free; /**< free semaphore */
    sem_t *used; /**< used semaphore */
} semaphores_t;

typedef struct edge {
    int u;
    int v;
} edge_t;

typedef struct solution {
    size_t size;
    edge_t edges[SOLUTION_MAX_EDGES];
} solution_t;

/**
 * Creates and maps the shared memory used in this program
 *
 * @brief Creates shared memory at SHM_NAME and with permissions PERMISSION
 * @details This function will generate an error if the shared memory was already created
 * @return The creates shared memory
 */
circular_buffer_t *memory_create(void);

/**
 * Unlinks and destroys the shared memory used in this program
 *
 * @brief Unmaps and destroys the created memory instance
 * @param shared The memory to destroy
 */
void memory_destroy(circular_buffer_t *shared);

/**
 * Opens existing shared memory instance
 *
 * @brief Opens existing shared memory. The memory must have been created before of this function will error
 * @return The opened memory
 */
circular_buffer_t *memory_open(void);

/**
 * Closes an open shared memory instance
 *
 * @brief Closes an open shared memory instance but does not destroy it
 * @param shared The memory to close
 */
void memory_close(circular_buffer_t *shared);

/**
 * @brief DEBUG: Prints the content of the given shared memory object
 * @param shared The memory to print
 */
void print_memory(circular_buffer_t *shared);

/**
 * Creates an instance of auth_semaphores_t containing the required semaphores
 *
 * @brief Creates an instance of auth_semaphores_t and sets up the semaphore links for server, client and mutex
 * @details This function will error if the semaphores were already created
 * @return A pointer to the allocated structure
 */
semaphores_t *semaphores_create(void);

/**
 * Destroys the used semaphores
 *
 * @brief Unlinks and destroys the semaphores, then frees the container
 * @param semaphores The semaphores to destroy
 */
void semaphores_destroy(semaphores_t *semaphores);

/**
 * Opens the existing semaphores and maps them together
 *
 * @brief The semaphores to open must have been created before
 * @return The opened semaphores
 */
semaphores_t *semaphores_open(void);

/**
 * Closes the opened semaphores
 *
 * @brief Closes the opened semaphores, but does not destroy them
 * @param semaphores The semaphores to close
 */
void semaphores_close(semaphores_t *semaphores);

/**
 * @brief Prints the given reason to stderr and exits with EXIT_FAILURE
 * @param reason Reason to print
 */
void error_exit(char *reason);

#endif //UE3_COMMON_H
