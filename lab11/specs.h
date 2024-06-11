#ifndef SPECS_H
#define SPECS_H

#include <netinet/in.h>

#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024
#define ID_LENGTH 32

typedef struct {
    int socket;
    char id[ID_LENGTH];
    struct sockaddr_in address;
    int addr_len;
    int active;
} client_t;

#endif
