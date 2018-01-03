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

node_t *create_node(const char *username, const char *password, const char *message) {
    node_t *node = malloc(sizeof(node_t));
    if (node == NULL) {
        // error
        return NULL;
    }
    node->username = malloc(sizeof(char) * (USERNAME_LENGTH + 1));
    node->password = malloc(sizeof(char) * (PASSWORD_LENGTH + 1));
    node->message = malloc(sizeof(char) * (MESSAGE_LENGTH + 1));
    if (node->username == NULL || node->password == NULL || node->message == NULL) {
        // error
    }
    strcpy(node->username, username);
    node->username[USERNAME_LENGTH] = '\0';
    strcpy(node->password, password);
    node->password[PASSWORD_LENGTH] = '\0';
    strcpy(node->message, message);
    node->message[MESSAGE_LENGTH] = '\0';

    return node;
}

void destroy_node(node_t *node) {
    if (node->next != NULL) destroy_node(node->next);
    free(node->username);
    free(node->password);
    free(node->message);
    free(node);
}

void write_node(node_t *node, char *file_name) {
    FILE *fp = fopen(file_name, "w");

    if (fp == NULL) {
        // error
    }

    while (node != NULL) {
        fprintf(fp, "%s%s%s%s%s\n", node->username, CSV_DELIMITER, node->password, CSV_DELIMITER, node->message);
        node = node->next;
    }
}

node_t *read_node(char *file_name) {
    node_t *head = NULL;
    FILE *fp = fopen(file_name, "r");
    char *line = NULL, *tok, username[USERNAME_LENGTH + 1], password[PASSWORD_LENGTH + 1], message[MESSAGE_LENGTH + 1];
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
        strcpy(username, tok);
        username[USERNAME_LENGTH] = '\0';

        tok = strtok(NULL, CSV_DELIMITER);
        if (tok == NULL) {
            // error
        }
        strcpy(password, tok);
        password[PASSWORD_LENGTH] = '\0';

        tok = strtok(NULL, CSV_DELIMITER);
        if (tok == NULL) {
            // error
        }
        strcpy(message, tok);
        message[strcspn(message, "\n")] = 0;

        if (head == NULL) head = create_node(username, password, message);
        else append_node(head, create_node(username, password, message));
    }

    fclose(fp);
    free(line); // Free, like specified in man pages

    return head;
}

void print_node(node_t *node) {
    printf("Printing node:\n");
    while (node != NULL) {
        printf("%s%s%s%s%s\n", node->username, CSV_DELIMITER, node->password, CSV_DELIMITER, node->message);
        node = node->next;
    }
    printf("Done!\n");
}