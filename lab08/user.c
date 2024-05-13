#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>


#include "config.h"

void generate_message(char *message, int size) {
    srand(getpid());
    char random_char = 'a' + rand() % 26;
    for (int i = 0; i < size - 1; i++) {
        message[i] = random_char;
    }
    message[size - 1] = '\0';
}


typedef struct {
    char messages[QUEUE_SIZE][MAX_MSG_SIZE];
    int head;
    int tail;
    int count;
} SharedQueue;

int main() {
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }

    SharedQueue *queue = mmap(NULL, sizeof(SharedQueue), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (queue == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    sem_t *sem_full = sem_open(SEM_FULL, 0);
    sem_t *sem_empty = sem_open(SEM_EMPTY, 0);
    sem_t *sem_mutex = sem_open(SEM_MUTEX, 0);

    if (sem_full == SEM_FAILED || sem_empty == SEM_FAILED || sem_mutex == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    char message[MAX_MSG_SIZE];
    generate_message(message, MAX_MSG_SIZE);
    sem_wait(sem_empty);
    sem_wait(sem_mutex);

    strcpy(queue->messages[queue->tail], message);
    queue->tail = (queue->tail + 1) % QUEUE_SIZE;
    queue->count++;

    sem_post(sem_mutex);
    sem_post(sem_full);

    printf("Message sent: %s\n", message);

    munmap(queue, sizeof(SharedQueue));
    close(shm_fd);
    sem_close(sem_full);
    sem_close(sem_empty);
    sem_close(sem_mutex);

    return 0;
}
