#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

void handleUSR1(int signum) {
    printf("Obsluga sygnalu SIGUSR1\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Użycie: %s [none|ignore|handler|mask]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "none") == 0) {
        printf("Sygnal SIGUSR1 nie jest obslugiwany\n");
    } else if (strcmp(argv[1], "ignore") == 0) {
        signal(SIGUSR1, SIG_IGN);
    } else if (strcmp(argv[1], "handler") == 0) {
        signal(SIGUSR1, handleUSR1);
    } else if (strcmp(argv[1], "mask") == 0) {
        sigset_t sigset;
        sigemptyset(&sigset);
        sigaddset(&sigset, SIGUSR1);
        sigprocmask(SIG_BLOCK, &sigset, NULL);
    } else {
        fprintf(stderr, "Nieznany tryb: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    raise(SIGUSR1);

    if (strcmp(argv[1], "mask") == 0) {
        sigset_t pending;
        sigemptyset(&pending);
        sigpending(&pending);

        if (sigismember(&pending, SIGUSR1)) {
            printf("Sygnał SIGUSR1 jest maskowany i oczekuje na dostarczenie\n");
        } else {
            printf("Sygnał SIGUSR1 nie jest w stanie oczekiwania\n");
        }
    }

    return EXIT_SUCCESS;

}