#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

double f(double x) {
    return 4/(x*x + 1);
}

double calculate_integral(double start, double end, double width) {
    double result = 0;
    double x = start;

    while (x < end) {
        result += f(x) * width;
        x += width;
    }

    return result;
}

int main() {
    mkfifo("pipe", 0666);

    int fd = open("pipe", O_RDONLY);
    double start, end;
    read(fd, &start, sizeof(start));
    read(fd, &end, sizeof(end));
    close(fd);

    double result = calculate_integral(start, end, 0.01);

    fd = open("pipe", O_WRONLY);
    write(fd, &result, sizeof(result));
    close(fd);

    return 0;
}
