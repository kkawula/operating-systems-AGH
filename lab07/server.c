#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#include "specs.h"


bool running = true;

void handlesig(int signum) {
  running = false;
}

int main(int argc, char *argv[]) {

  struct mq_attr attr = {
    .mq_maxmsg = 10,
    .mq_msgsize = sizeof(message),
    .mq_flags = 0
  };

  mqd_t server_queue = mq_open(SERVER_NAME, O_RDWR | O_CREAT,  S_IRUSR | S_IWUSR, &attr);
  if (server_queue == -1) {
    perror("mq_open");
    exit(EXIT_FAILURE);
  }


  msd_t client_queue[MAX_CLIENTS];

  for (int i = 0; i < MAX_CLIENTS; i++) {
    client_queue[i] = -1;
  }

  for (int sig = 0; sig < SIGRTMAX; sig++) {
    signal(sig, handlesig);
  }

  message msg;

  while (running) {
   
    mq_receive(server_queue, (char *) &msg, sizeof(message), NULL);

    switch (msg.type) {
      case INIT:
        int i = 0;
        while (i < MAX_CLIENTS && client_queue[i] != -1) {
          i++;
        }
        if (i == MAX_CLIENTS) {
          fprintf(stderr, "There is no space for new client\n");
          break;
        }
        client_queue[i] = mq_open(msg.data, O_WRONLY);
        if (client_queue[i] == -1) {
          perror("mq_open");
        }

        message response = {
          .type = IDENTIFIER,
          .id = i
        };

        mq_send(client_queue[i], (char *) &response, sizeof(message), 10);
        printf("Client %d connected\n", i);
        break;
      case MESSAGE:
        for (int i = 0; i < MAX_CLIENTS; i++) {
          if (client_queue[i] != -1 && client_queue[i] != msg.id) {
            mq_send(client_queue[i], (char *) &msg, sizeof(message), 10);
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
        exit(EXIT_FAILURE);
    }
  }

  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (client_queue[i] != -1) {
      mq_close(client_queue[i]);
    }
  }

  mq_close(server_queue);
  mq_unlink(SERVER_NAME);



  return 0;
}
