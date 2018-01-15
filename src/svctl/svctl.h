#ifndef UE4_SVCTL_H
#define UE4_SVCTL_H

#define KEY_SIZE 10

typedef struct sv_message {
    int id;
    int size;
    char key[KEY_SIZE];
} sv_message_t;

#endif //UE4_SVCTL_H
