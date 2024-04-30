#include "zad3.h"

void readwrite(int pd, size_t block_size);

void createpipe(size_t block_size)
{
    /* Utwórz potok nienazwany */
    
    int fd[2];
    pipe(fd);

    /* Odkomentuj poniższe funkcje zamieniając ... na deskryptory potoku */
    check_pipe(fd);
    check_write(fd, block_size, readwrite);
}

void readwrite(int write_pd, size_t block_size)
{
    /* Otworz plik `unix.txt`, czytaj go po `block_size` bajtów
    i w takich `block_size` bajtowych kawałkach pisz do potoku `write_pd`.*/

    FILE *file = fopen("unix.txt", "r");
    char buffer[block_size];
    size_t size;
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);
    size_t bytes_to_read;


    while (size > 0) {
        bytes_to_read = size < block_size ? size : block_size;
        size_t read = fread(buffer, sizeof(char), bytes_to_read, file);
        write(write_pd, buffer, read);
        size -= read;
    }
    /* Zamknij plik */
    fclose(file);
}

int main()
{
    srand(42);
    size_t block_size = rand() % 128;
    createpipe(block_size);

    return 0;
}