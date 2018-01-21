#ifndef UE4_SECVAULT_H
#define UE4_SECVAULT_H

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/semaphore.h>
#include "common.h"
#include "data.h"
#include "ctl.h"

// log

void alert(const char *message);
void alerti(const char *message, int arg);

// dev

#define DEVICE_MAJOR (231)
#define DEVICE_CTL_MINOR (0)

#define DEVICE_DATA_AMOUNT 4

typedef struct sv_dev {
    struct cdev cdev;
    struct semaphore sem;
    unsigned int id;
    char *content;
    char key[KEY_SIZE];
    unsigned int current_size;
    unsigned int max_size;
    uid_t user;
} sv_dev_t;

#endif //UE4_SECVAULT_H
