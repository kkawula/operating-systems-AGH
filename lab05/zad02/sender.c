#define _XOPEN_SOURCE 700

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int confirmation_received = 0;

void handleSIGUSR1(int signum) {
    printf("Confirmation - received SIGUSR1\n");
    confirmation_received = 1;
}

int main(int argc, char *argv[]) {

    // argc[1] - catcher_pid
    // argc[2] - mode

    printf("Sender PID: %d\n", getpid());

    pid_t catcher_pid = (pid_t)atoi(argv[1]);
    int mode = atoi(argv[2]);

    printf("Catcher PID: %d\n", catcher_pid);
    printf("Mode: %d\n", mode);

    // struct sigaction act;
    // act.sa_handler = handleSIGUSR1;
    // sigemptyset(&act.sa_mask);
    // act.sa_flags = 0;

    // sigaction(SIGUSR1, &act, NULL);

    union sigval value = {mode};

    sigqueue(catcher_pid, SIGUSR1, value);

    // while (!confirmation_received) {
    //     pause();
    // }

    return 0;
}
