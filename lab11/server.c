#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>
#include "specs.h"

client_t *clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void add_client(client_t *cl);
void remove_client(int uid);
void send_message(char *message, int uid);
void *handle_client(void *arg);
void *alive_ping(void *arg);
void print_client_addr(struct sockaddr_in addr);

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <address> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *address = argv[1];
    int port = atoi(argv[2]);

    int server_socket, new_socket;
    struct sockaddr_in server_addr, client_addr;
    pthread_t tid, ping_tid;

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(address);
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 10) < 0) {
        perror("listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server started on %s:%d\n", address, port);

    pthread_create(&ping_tid, NULL, alive_ping, NULL);

    while (1) {
        socklen_t clilen = sizeof(client_addr);
        if ((new_socket = accept(server_socket, (struct sockaddr *)&client_addr, &clilen)) < 0) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }

        client_t *cli = (client_t *)malloc(sizeof(client_t));
        cli->address = client_addr;
        cli->socket = new_socket;
        cli->addr_len = clilen;
        cli->active = 1;

        pthread_mutex_lock(&clients_mutex);
        for (int i = 0; i < MAX_CLIENTS; ++i) {
            if (!clients[i]) {
                clients[i] = cli;
                break;
            }
        }
        pthread_mutex_unlock(&clients_mutex);

        pthread_create(&tid, NULL, handle_client, (void *)cli);
    }

    return 0;
}


void *handle_client(void *arg) {
    char buffer[BUFFER_SIZE];
    char id[ID_LENGTH];
    int leave_flag = 0;

    client_t *cli = (client_t *)arg;

    if (recv(cli->socket, id, ID_LENGTH, 0) <= 0 || strlen(id) <  2 || strlen(id) >= ID_LENGTH) {
        printf("Didn't enter the name.\n");
        leave_flag = 1;
    } else {
        strcpy(cli->id, id);
        sprintf(buffer, "%s has joined\n", cli->id);
        printf("%s", buffer);
        send_message(buffer, cli->socket);
    }

    bzero(buffer, BUFFER_SIZE);

    while (1) {
        if (leave_flag) {
            break;
        }

        int receive = recv(cli->socket, buffer, BUFFER_SIZE, 0);
        if (receive > 0) {
            if (strlen(buffer) > 0) {

                char command[BUFFER_SIZE];
                sscanf(buffer, "%s", command);
                printf("Command:%s\n", buffer);

                if (strcmp(command, "LIST") == 0) {
                    pthread_mutex_lock(&clients_mutex);
                    char list[BUFFER_SIZE] = "Active clients:\n";
                    for (int i = 0; i < MAX_CLIENTS; ++i) {
                        if (clients[i]) {
                            strcat(list, clients[i]->id);
                            strcat(list, "\n");
                        }
                    }
                    pthread_mutex_unlock(&clients_mutex);
                    write(cli->socket, list, strlen(list));
                } else if (strncmp(command, "2ALL", 4) == 0) {
                    char message[BUFFER_SIZE];
                    sprintf(message, "%s: %s", cli->id, buffer + 5);
                    send_message(message, cli->socket);
                } else if (strncmp(command, "2ONE", 4) == 0) {
                    char dest_id[ID_LENGTH], message[BUFFER_SIZE];
                    sscanf(buffer, "2ONE %s %[^\n]", dest_id, message);
                    pthread_mutex_lock(&clients_mutex);
                    for (int i = 0; i < MAX_CLIENTS; ++i) {
                        if (clients[i] && strcmp(clients[i]->id, dest_id) == 0) {
                            write(clients[i]->socket, message, strlen(message));
                            break;
                        }
                    }
                    pthread_mutex_unlock(&clients_mutex);
                } else if (strcmp(command, "STOP") == 0) {
                    sprintf(buffer, "%s has left\n", cli->id);
                    send_message(buffer, cli->socket);
                    leave_flag = 1;
                } else {
                    printf("Invalid command\n");
                }
            }
        } else if (receive == 0 || strcmp(buffer, "STOP") == 0) {
            sprintf(buffer, "%s has left\n", cli->id);
            printf("%s", buffer);
            send_message(buffer, cli->socket);
            leave_flag = 1;
        } else {
            perror("recv failed");
            leave_flag = 1;
        }

        bzero(buffer, BUFFER_SIZE);
    }

    close(cli->socket);
    remove_client(cli->socket);
    free(cli);
    pthread_detach(pthread_self());

    return NULL;
}


void add_client(client_t *cl) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (!clients[i]) {
            clients[i] = cl;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void remove_client(int uid) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i]) {
            if (clients[i]->socket == uid) {
                clients[i] = NULL;
                break;
            }
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void send_message(char *message, int uid) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i]) {
            if (clients[i]->socket != uid) {
                if (write(clients[i]->socket, message, strlen(message)) < 0) {
                    perror("ERROR: write to descriptor failed");
                    break;
                }
            }
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void *alive_ping(void *arg) {
    while (1) {
        sleep(10);

        pthread_mutex_lock(&clients_mutex);
        for (int i = 0; i < MAX_CLIENTS; ++i) {
            if (clients[i]) {
                if (write(clients[i]->socket, "ALIVE", 5) < 0) {
                    printf("Client %s disconnected\n", clients[i]->id);
                    close(clients[i]->socket);
                    clients[i] = NULL;
                }
            }
        }
        pthread_mutex_unlock(&clients_mutex);
    }
}
