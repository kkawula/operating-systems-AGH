#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#include "specs.h"


volatile bool running = true;

void handlesig(int signum) {
  running = false;
}

int main(int argc, char *argv[]) {

  pid_t pid = getpid();
  char queue_name[40] = {0};
  sprintf(queue_name, "/client_queue_%d", pid);

  struct mq_attr attr = {
    .mq_maxmsg = 10,
    .mq_msgsize = sizeof(message),
    .mq_flags = 0
  };

  mqd_t client_queue = mq_open(queue_name, O_RDWR | O_CREAT,  S_IRUSR | S_IWUSR, &attr);
  if (client_queue == -1) {
    perror("mq_open client");
    exit(EXIT_FAILURE);
  }

  mqd_t server_queue = mq_open(SERVER_NAME, O_RDWR | O_CREAT,  S_IRUSR | S_IWUSR);
  if (server_queue == -1) {
    printf("Server not found\n");
    perror("mq_open server");
  }


  message message_init = {
    .type = INIT,
    .id = -1
  };

  memcpy(message_init.text, queue_name, strlen(queue_name));

  if(mq_send(server_queue, (char*)&message_init, sizeof(message_init), 10) < 0){
    perror("mq_send init");
  }

  int to_parent_pipe[2];
  if(pipe(to_parent_pipe) < 0)
      perror("pipe");

  for (int sig = 1; sig < SIGRTMAX; sig++) {
      signal(sig, handlesig);
  }

  pid_t listener_pid = fork();
  if (listener_pid < 0)
    perror("fork listener");
  else if (listener_pid == 0) {
    close(to_parent_pipe[0]);
    message receive_message;

    while(running) {
      mq_receive(mq_client_descriptor, (char*)&receive_message, sizeof(receive_message), NULL);
      switch(receive_message.type) {
        case MESSAGE:
          printf("Received from id: %d message: %s\n", receive_message.id, receive_message.data);
          break;
        case IDENTIFIER:
          printf("Received identifier from server: %d\n", receive_message.id);
          write(to_parent_pipe[1], &receive_message.id, sizeof(receive_message.id));
          break;
        default:
          printf("Unknown message type in client queue: %d", receive_message.type);
          break;
      }
    }
    printf("Exiting from receive loop\n");
    exit(0);
  } else {
    close(to_parent_pipe[1]);
    int identifier = -1;

    if(read(to_parent_pipe[0], &identifier, sizeof(identifier)) < 0)
      perror("read identifier");

    char* buffer = NULL;
    while(running) {
      mq_getattr(server_queue, &attributes);
      if(attributes.mq_curmsgs >= attributes.mq_maxmsg) {
        printf("Server is busy, please wait\n");
        continue;
      }
      
      if(scanf("%ms", &buffer) == 1) {
        message_t send_message = {
          .type = MESSAGE,
          .it = identifier
        };
        memcpy(send_message.text, buffer, strlen(buffer));

        mq_send(server_queue, (char*)&send_message, sizeof(send_message), 10);

        free(buffer);
        buffer = NULL;
      } else
          perror("scanf input");
    }

    printf("Exiting from sending loop\n");


    if(identifier != -1){
      message message_close = {
        .type = CLIENT_CLOSE,
        .id = identifier
      };
      mq_send(server_queue, (char*)&message_close, sizeof(message_close), 10);
    }

    mq_close(server_queue);
    mq_close(server_queue);

    mq_unlink(queue_name);
  }
  return 0;
}
