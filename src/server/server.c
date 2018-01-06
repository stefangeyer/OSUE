#include <stdio.h>
#include <string.h>
#include "server.h"

node_t *append_node(node_t *head, node_t *child) {
    if (head == NULL) {
        return child;
    } else {
        node_t *curr = head;
        while (curr->next != NULL) curr = curr->next;
        curr->next = child;
        return head;
    }
}

node_t *create_node(const char *username, const char *password, const char *secret) {
    node_t *node = malloc(sizeof(node_t));
    if (node == NULL) {
        // error
        return NULL;
    }
    node->username = malloc(sizeof(char) * (USERNAME_LENGTH + 1));
    node->password = malloc(sizeof(char) * (PASSWORD_LENGTH + 1));
    node->secret = malloc(sizeof(char) * (SECRET_LENGTH + 1));
    node->session = malloc(sizeof(char) * (SESSION_LENGTH + 1));
    if (node->username == NULL || node->password == NULL || node->secret == NULL) {
        // error
    }
    strncpy(node->username, username, USERNAME_LENGTH);
    node->username[USERNAME_LENGTH] = 0;
    strncpy(node->password, password, PASSWORD_LENGTH);
    node->password[PASSWORD_LENGTH] = 0;
    strncpy(node->secret, secret, SECRET_LENGTH);
    node->secret[SECRET_LENGTH] = 0;

    return node;
}

void destroy_node(node_t *node) {
    if (node->next != NULL) destroy_node(node->next);
    free(node->username);
    free(node->password);
    free(node->secret);
    free(node->session);
    free(node);
}

node_t *search_node_for(node_t *node, int field, char *value) {
    while (node != NULL) {
        switch (field) {
            case FIELD_USERNAME:
                if (strcmp(node->username, value) == 0)
                    return node;
                break;
            case FIELD_PASSWORD:
                if (strcmp(node->password, value) == 0)
                    return node;
                break;
            case FIELD_SECRET:
                if (strcmp(node->secret, value) == 0)
                    return node;
                break;
            case FIELD_SESSION:
                if (strcmp(node->session, value) == 0)
                    return node;
                break;
            default:
                // error
                break;
        }

        node = node->next;
    }

    return NULL;
}

void write_node(node_t *node, char *file_name) {
    FILE *fp = fopen(file_name, "w");

    if (fp == NULL) {
        // error
    }

    while (node != NULL) {
        fprintf(fp, "%s%s%s%s%s\n", node->username, CSV_DELIMITER, node->password, CSV_DELIMITER, node->secret);
        node = node->next;
    }
}

node_t *read_node(char *file_name) {
    node_t *head = NULL;
    FILE *fp = fopen(file_name, "r");
    char *line = NULL, *tok, username[USERNAME_LENGTH], password[PASSWORD_LENGTH], secret[SECRET_LENGTH];
    size_t len = 0;

    if (fp == NULL) {
        // Cannot open file -> return empty head (NULL)
        return NULL;
    }

    while (getline(&line, &len, fp) != -1) {
        tok = strtok(line, CSV_DELIMITER);
        if (tok == NULL) {
            // error
        }
        strncpy(username, tok, USERNAME_LENGTH);
        username[USERNAME_LENGTH] = 0;

        tok = strtok(NULL, CSV_DELIMITER);
        if (tok == NULL) {
            // error
        }
        strncpy(password, tok, PASSWORD_LENGTH);
        password[PASSWORD_LENGTH] = 0;

        tok = strtok(NULL, CSV_DELIMITER);
        if (tok == NULL) {
            // error
        }
        strncpy(secret, tok, SECRET_LENGTH);
        secret[SECRET_LENGTH] = 0;
        secret[strcspn(secret, "\n")] = 0;

        if (head == NULL) head = create_node(username, password, secret);
        else append_node(head, create_node(username, password, secret));
    }

    fclose(fp);
    free(line); // Free, like specified in man pages

    return head;
}

void print_node(node_t *node) {
    printf("Printing node:\n");
    while (node != NULL) {
        printf("%s%s%s%s%s\n", node->username, CSV_DELIMITER, node->password, CSV_DELIMITER, node->secret);
        node = node->next;
    }
    printf("Done!\n");
}