#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

volatile int mode = 0;
volatile int counter = 0;
volatile int sender_pid = 0;

void sendConfirmation(int sender_pid) {
    union sigval value;
    value.sival_int = 1;
    sigqueue(sender_pid, SIGUSR1, value);
}

void handleSIGUSR1(int sig, siginfo_t *info, void *ucontext) {
    mode = info->si_value.sival_int;
    sender_pid = info->si_pid;
    counter++;
    printf("Received signal from sender PID: %d\n", sender_pid);

    sendConfirmation(sender_pid);

    if (mode == 1) {
        for (int i = 1; i <= 100; ++i) {
            printf("%d\n", i);
        }
    } else if (mode == 2) {
        printf("Liczba otrzymanych żądań zmiany trybu pracy: %d\n", counter);
    } else if (mode == 3) {
        exit(EXIT_SUCCESS);
    } else {
        printf("Received signal with unknown mode: mode %d\n", mode);
        printf("Unknown mode\n");
    }
}



int main(void) {
    printf("Catcher PID: %d\n", getpid());

    struct sigaction act;
    act.sa_sigaction = handleSIGUSR1;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;

    sigaction(SIGUSR1, &act, NULL);

    while (1) {
        pause();
    }
}
