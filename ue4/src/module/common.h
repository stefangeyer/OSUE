#ifndef UE4_COMMON_H
#define UE4_COMMON_H

// ctl data

#define KEY_SIZE (10)

typedef struct message {
    unsigned int id;
    unsigned int size;
    char key[KEY_SIZE];
} ctl_message_t;

// ctl commands

#define CMD_CREATE 0
#define CMD_CHANGE_KEY 1
#define CMD_SIZE 2
#define CMD_CLEAR 3 // Clear content
#define CMD_REMOVE 4
#define CMD_MAX 4

#endif //UE4_COMMON_H
