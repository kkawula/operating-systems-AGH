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

    while (1) {
        char buffer[MAX_MSG_SIZE];

        // Czekanie na pełną kolejkę
        sem_wait(sem_full);
        // Uzyskanie wyłącznego dostępu do kolejki
        sem_wait(sem_mutex);

        // Odbieranie wiadomości z kolejki
        strcpy(buffer, queue->messages[queue->head]);
        queue->head = (queue->head + 1) % QUEUE_SIZE;
        queue->count--;

        // Zwolnienie dostępu do kolejki
        sem_post(sem_mutex);
        // Zwiększenie licznika pustych miejsc w kolejce
        sem_post(sem_empty);

        // Wypisywanie znaków wiadomości co sekundę
        printf("Received message: ");
        for (int i = 0; i < strlen(buffer); i++) {
            putchar(buffer[i]);
            fflush(stdout);  // Wymuszenie wypisania znaku
            sleep(1);        // Opóźnienie 1 sekundy
        }
        printf("\n");
    }

    munmap(queue, sizeof(SharedQueue));
    close(shm_fd);
    sem_close(sem_full);
    sem_close(sem_empty);
    sem_close(sem_mutex);

    return 0;
}