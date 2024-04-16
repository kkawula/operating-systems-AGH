#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    double start, end;
    printf("Podaj przedział całkowania (start end): ");
    scanf("%lf %lf", &start, &end);

    int fd = open("pipe", O_WRONLY);
    write(fd, &start, sizeof(start));
    write(fd, &end, sizeof(end));
    close(fd);

    fd = open("pipe", O_RDONLY);
    double result;
    read(fd, &result, sizeof(result));
    close(fd);

    printf("Wynik: %f\n", result);

    return 0;
}