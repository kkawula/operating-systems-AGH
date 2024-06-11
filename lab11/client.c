#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include "specs.h"

volatile sig_atomic_t flag = 0;
int sockfd = 0;
char id[ID_LENGTH];

void catch_ctrl_c_and_exit(int sig) {
    flag = 1;
}

void send_message_handler() {
    char buffer[BUFFER_SIZE] = {};
    char message[BUFFER_SIZE + ID_LENGTH] = {};

    while (1) {
        printf("> ");
        fflush(stdout);
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strlen(buffer) - 1] = '\0';

        if (strcmp(buffer, "STOP") == 0) {
            send(sockfd, buffer, strlen(buffer), 0);
            break;
        } else {
            sprintf(message, "%s", buffer);
            send(sockfd, message, strlen(message), 0);
        }

        bzero(buffer, BUFFER_SIZE);
        bzero(message, BUFFER_SIZE + ID_LENGTH);
    }
    catch_ctrl_c_and_exit(2);
}

void recv_message_handler() {
    char message[BUFFER_SIZE] = {};
    while (1) {
        int receive = recv(sockfd, message, BUFFER_SIZE, 0);
        if (receive > 0) {
            if (strcmp(message, "ALIVE") == 0) {
                continue;
            }
            printf("%s\n", message);
            printf("> ");
            fflush(stdout);
        } else if (receive == 0) {
            break;
        } else {
            perror("ERROR: recv");
        }
        memset(message, 0, sizeof(message));
    }
}

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <ID> <address> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, catch_ctrl_c_and_exit);

    strcpy(id, argv[1]);
    char *address = argv[2];
    int port = atoi(argv[3]);

    struct sockaddr_in server_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("ERROR: socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(address);
    server_addr.sin_port = htons(port);

    int err = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (err == -1) {
        perror("ERROR: connect");
        exit(EXIT_FAILURE);
    }

    send(sockfd, id, ID_LENGTH, 0);

    printf("=== WELCOME TO THE CHATROOM ===\n");

    pthread_t send_msg_thread;
    if (pthread_create(&send_msg_thread, NULL, (void *)send_message_handler, NULL) != 0) {
        perror("ERROR: pthread");
        exit(EXIT_FAILURE);
    }

    pthread_t recv_msg_thread;
    if (pthread_create(&recv_msg_thread, NULL, (void *)recv_message_handler, NULL) != 0) {
        perror("ERROR: pthread");
        exit(EXIT_FAILURE);
    }

    while (1) {
        if (flag) {
            printf("\nBye\n");
            break;
        }
    }

    close(sockfd);

    return 0;
}
