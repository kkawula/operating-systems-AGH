#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>

#include "config.h"

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

    sem_t *sem_any = sem_open(SEM_ANY, 0);
    sem_t *sem_empty = sem_open(SEM_EMPTY, 0);
    sem_t *sem_mutex = sem_open(SEM_MUTEX, 0);

    if (sem_any == SEM_FAILED || sem_empty == SEM_FAILED || sem_mutex == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    while (1) {
        char buffer[MAX_MSG_SIZE];

        sem_wait(sem_any);
        sem_wait(sem_mutex);

        strcpy(buffer, queue->messages[queue->head]);
        queue->head = (queue->head + 1) % QUEUE_SIZE;

        sem_post(sem_mutex);
        sem_post(sem_empty);

        printf("Received message: ");
        for (int i = 0; i < strlen(buffer); i++) {
            putchar(buffer[i]);
            fflush(stdout);
            sleep(1);
        }
        printf("\n");
    }

    munmap(queue, sizeof(SharedQueue));
    close(shm_fd);
    sem_close(sem_any);
    sem_close(sem_empty);
    sem_close(sem_mutex);

    return 0;
}