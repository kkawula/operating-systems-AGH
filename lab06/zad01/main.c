#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


double f(double x) {
    return 4/(x*x + 1);
}

double calculate_segment(double start, double end, double width) {
    double result = 0;
    double x = start;

    while (x < end) {
        result += f(x) * width;
        x += width;
    }

    return result;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Użycie: %s [width] [nuber of processes] \n", argv[0]);
        exit(EXIT_FAILURE);
    }

    float width = atof(argv[1]);
    int number_of_processes = atoi(argv[2]);

    double result = 0;
    int fd[2];
    if (pipe(fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    double start = 0;
    double end = 1;
    for (int i = 0; i < number_of_processes - 1; ++i) {
        pid_t pid = fork();

        if (pid == 0) {
            close(fd[0]);
            double segment_result = calculate_segment(start + i* end/number_of_processes , start + (i + 1)* end/number_of_processes, width);
            write(fd[1], &segment_result, sizeof(segment_result));
            close(fd[1]);
            exit(EXIT_SUCCESS);
        }
    }

    close(fd[1]);

    for (int i = 0; i < number_of_processes; ++i) {
        double segment_result;
        read(fd[0], &segment_result, sizeof(segment_result));
        result += segment_result;
    }

    close(fd[0]);

    printf("Result: %f\n", result);

    return EXIT_SUCCESS;

}