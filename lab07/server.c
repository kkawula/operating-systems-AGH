#include <stdio.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <signal.h>
#include <stdlib.h>

#include "specs.h"


volatile bool running = true;

void handleSIG(int signum) {
  running = false;
  printf("Server closing\n");
}

int main() {

  struct mq_attr attr = {
    .mq_maxmsg = 10,
    .mq_msgsize = sizeof(message),
    .mq_flags = 0
  };

  mqd_t server_queue = mq_open(SERVER_NAME, O_RDWR | O_CREAT,  S_IRUSR | S_IWUSR, &attr);
  if (server_queue == -1) {
    perror("mq_open server fail");
    exit(EXIT_FAILURE);
  }


  mqd_t client_queue[MAX_CLIENTS];

  for (int i = 0; i < MAX_CLIENTS; i++) {
    client_queue[i] = -1;
  }

  for (int sig = 1; sig < SIGRTMAX; sig++) {
    signal(sig, handleSIG);
  }

  message msg;

  while (running) {
    printf("Waiting for message\n");
    mq_receive(server_queue, (char *) &msg, sizeof(message), NULL);
    printf("Received message\n");
    switch (msg.type) {
      case INIT:
        int id = 0;
        while (id < MAX_CLIENTS && client_queue[id] != -1) {
          id++;
        }
        if (id == MAX_CLIENTS) {
          fprintf(stderr, "There is no space for new client\n");
          break;
        }
        client_queue[id] = mq_open(msg.data, O_RDWR, S_IRUSR | S_IWUSR, NULL);
        if (client_queue[id] == -1) {
          perror("mq_open");
        }

        message response = {
          .type = IDENTIFIER,
          .id = id
        };

        mq_send(client_queue[id], (char *) &response, sizeof(message), 10);
        printf("Client %d connected\n", id);
        break;
      case MESSAGE:
        for (int i = 0; i < MAX_CLIENTS; i++) {
          if (client_queue[i] != -1 && client_queue[i] != msg.id) {
            if(mq_send(client_queue[i], (char *) &msg, sizeof(message), 10) != 0) {
              perror("mq_send broadcast");
            }
          }
        }
        break;
      case CLIENT_CLOSE:
        mq_close(client_queue[msg.id]);
        client_queue[msg.id] = -1;
        printf("Client %d disconnected\n", msg.id);
        break;
      default:
        fprintf(stderr, "Unknown message type\n");
        break;
    }
  }

  printf("Out of loop - Closing server\n");

  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (client_queue[i] != -1) {
      mq_close(client_queue[i]);
    }
  }

  mq_close(server_queue);
  mq_unlink(SERVER_NAME);



  return 0;
}
