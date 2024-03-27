#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int global = 4;

int main(int argc, char *argv[]) {
    if(argc != 2) {
        printf("Podaj sciezke katalogu jako argument\n");
        return 1;
    }

    int local = 0;
    pid_t pid = fork();

    if(pid < 0) {
        printf("Blad przy tworzeniu procesu potomnego\n");
        return 1;
    }

    if(pid == 0) {
        printf("child process\n");
        global++;
        local++;
        printf("child pid = %d, parent pid = %d\n", (int)getpid(), (int)getppid());
        printf("child's local = %d, child's global = %d\n", local, global);
        int exec_status = execl("/bin/ls", "/bin/ls", argv[1], NULL);
        if(exec_status == -1) {
            printf("Exec error\n");
            return 1;
        }
    } else {
        int status;
        waitpid(pid, &status, 0);
        printf("parent process\n");
        printf("parent pid = %d, child pid = %d\n", (int)getpid(), (int)pid);
        printf("Child exit code: %d\n", WEXITSTATUS(status));
        printf("Parent's local = %d, parent's global = %d\n", local, global);
    }

    return 0;
}