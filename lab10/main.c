#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_REINDEER 9
#define NUM_DELIVERIES 4

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t santa_cond = PTHREAD_COND_INITIALIZER;
int reindeer_arrived = 0;
int deliveries = 0;

void* santa_thread(void* arg) {
    while (deliveries < NUM_DELIVERIES) {
        pthread_mutex_lock(&mutex);
        
        while (reindeer_arrived < NUM_REINDEER) {
            pthread_cond_wait(&santa_cond, &mutex);
        }
        
        printf("Mikołaj: budzę się\n");
        printf("Mikołaj: dostarczam zabawki\n");
        
        sleep(rand() % 3 + 2);
        
        reindeer_arrived = 0;
        deliveries++;
        
        printf("Mikołaj: zasypiam\n");
        
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void* reindeer_thread(void* arg) {
    int id = *(int*)arg;
    while (deliveries < NUM_DELIVERIES) {
        sleep(rand() % 6 + 5);
        
        pthread_mutex_lock(&mutex);
        
        reindeer_arrived++;
        printf("Renifer: czeka %d reniferów na Mikołaja, ID: %d\n", reindeer_arrived, id);
        
        if (reindeer_arrived == NUM_REINDEER) {
            printf("Renifer: wybudzam Mikołaja, ID: %d\n", id);
            pthread_cond_signal(&santa_cond);
        }
        
        pthread_mutex_unlock(&mutex);
        
        while (deliveries < NUM_DELIVERIES && reindeer_arrived > 0) {
            usleep(100000);
        }
    }
    return NULL;
}

int main() {
    pthread_t santa;
    pthread_t reindeers[NUM_REINDEER];
    int reindeer_ids[NUM_REINDEER];

    srand(time(NULL));

    pthread_create(&santa, NULL, santa_thread, NULL);

    for (int i = 0; i < NUM_REINDEER; i++) {
        reindeer_ids[i] = i;
        pthread_create(&reindeers[i], NULL, reindeer_thread, &reindeer_ids[i]);
    }

    pthread_join(santa, NULL);

    for (int i = 0; i < NUM_REINDEER; i++) {
        pthread_join(reindeers[i], NULL);
    }

    return 0;
}
