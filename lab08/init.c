#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>

#include "config.h"


typedef struct {
    char messages[QUEUE_SIZE][MAX_MSG_SIZE];
    int head;
    int tail;
    int count;
} SharedQueue;

int main() {
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }

    if (ftruncate(shm_fd, sizeof(SharedQueue)) == -1) {
        perror("ftruncate");
        exit(1);
    }

    SharedQueue *queue = mmap(NULL, sizeof(SharedQueue), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (queue == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    queue->head = 0;
    queue->tail = 0;
    queue->count = 0;

    sem_t *sem_any = sem_open(SEM_ANY, O_CREAT, 0666, 0);
    sem_t *sem_empty = sem_open(SEM_EMPTY, O_CREAT, 0666, QUEUE_SIZE);
    sem_t *sem_mutex = sem_open(SEM_MUTEX, O_CREAT, 0666, 1);

    if (sem_any == SEM_FAILED || sem_empty == SEM_FAILED || sem_mutex == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    munmap(queue, sizeof(SharedQueue));
    close(shm_fd);
    sem_close(sem_any);
    sem_close(sem_empty);
    sem_close(sem_mutex);

    return 0;
}
