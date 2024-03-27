#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if(argc != 2) {
        printf("Podaj liczbe procesow potomnych jako argument\n");
        return 1;
    }

    int n = atoi(argv[1]);
    pid_t pid;

    for(int i = 0; i < n; i++) {
        pid = fork();

        if(pid < 0) {
            printf("Blad przy tworzeniu procesu potomnego\n");
            return 1;
        }

        if(pid == 0) {
            printf("Proces dziecka: Proces rodzica ma pid:%d, Proces dziecka ma pid:%d\n", (int)getppid(), (int)getpid());
            return 0;
        }
    }

    for(int i = 0; i < n; i++) {
        wait(NULL);
    }

    printf("%s\n", argv[1]);

    return 0;
}