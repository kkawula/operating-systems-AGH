#define _XOPEN_SOURCE 700

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


volatile sig_atomic_t confirmation_received = 0;

void handleSIGUSR1(int signum) {
    printf("Confirmation - received SIGUSR1\n");
    confirmation_received = 1;
}

int main(int argc, char *argv[]) {

    // argc[1] - catcher_pid
    // argc[2] - mode

    pid_t catcher_pid = (pid_t)atoi(argv[1]);
    int mode = atoi(argv[2]);

    struct sigaction act;
    act.sa_handler = handleSIGUSR1;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGUSR1, &act, NULL);

    union sigval value;
    value.sival_int = mode;

    sigqueue(catcher_pid, SIGUSR1, value);

    while (!confirmation_received) {
        pause();
    }

    return 0;
}
