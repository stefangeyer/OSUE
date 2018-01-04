#include <string.h>
#include "client.h"

static char tmp_secret[SECRET_LENGTH] = "I am a message";

int signup(char *username, char *password) {
    return 0;
}

int login(char *username, char *password, char *session) {
    strcpy(session, "abc");
    return 0;
}

int logout(char *session) {
    return 0;
}

int read_secret(char *session, char *secret) {
    strcpy(secret, tmp_secret);
    return 0;
}

int write_secret(char *session, char *secret) {
    strcpy(tmp_secret, secret);
    tmp_secret[strcspn(tmp_secret, "\n")] = 0;
    return 0;
}