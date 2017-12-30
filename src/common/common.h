#ifndef UE3_COMMON_H
#define UE3_COMMON_H

#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

#define SHM_NAME "/myshm"
#define MAX_DATA (50)
#define PERMISSION (0600)

typedef struct myshm {
    unsigned int state;
    unsigned int data[MAX_DATA];
} myshm_t;

myshm_t *memory_create(void);
void memory_destroy(myshm_t *shared);

#endif //UE3_COMMON_H
