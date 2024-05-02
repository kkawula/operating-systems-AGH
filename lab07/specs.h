#ifndef SPECS_H
#define SPECS_H

#define SERVER_NAME "/localhost"
#define MAX_CLIENTS 10
#define MAX_MSG_SIZE 254

typedef enum {
    INIT,
    IDENTIFIER,
    MESSAGE,
    CLIENT_CLOSE,
} message_type;

typedef struct {
    message_type type;
    int id;
    char data[256];
} message;

#endif