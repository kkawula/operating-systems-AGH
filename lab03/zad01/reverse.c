#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BLOCK_SIZE 1024

void open_files(const char* input_file, const char* output_file, FILE **in, FILE **out) {
    if ((*in = fopen(input_file, "r")) == NULL) {
        perror("Cannot open input file");
        exit(EXIT_FAILURE);
    }

    if ((*out = fopen(output_file, "w")) == NULL) {
        perror("Cannot open output file");
        exit(EXIT_FAILURE);
    }

}

void reverse_byte(FILE*in, FILE *out) {
    int ch;

    if (fseek(in, -1, SEEK_END) != 0) {
        perror("Cannot seek to the end of file");
        exit(EXIT_FAILURE);
    }

    long offset = ftell(in);;

    while (offset >= 0) {
        fseek(in, offset, SEEK_SET);
        ch = fgetc(in);
        fputc(ch, out);
        offset--;
    }

    fclose(in);
    fclose(out);
}

void reverse_block(FILE* in, FILE* out) {
    char buffer[BLOCK_SIZE];
    size_t read_bytes, nitems;

    if (fseek(in, -1, SEEK_END) != 0) {
        perror("Cannot seek to the end of file");
        exit(EXIT_FAILURE);
    }

    long offset = ftell(in);

    while (offset > 0) {
        nitems = offset < BLOCK_SIZE ? offset : BLOCK_SIZE;
        fseek(in, offset - nitems, SEEK_SET);

        read_bytes = fread(buffer, sizeof(char), nitems, in);

        for (int i = 0; i < read_bytes / 2; i++) {
            char temp = buffer[i];
            buffer[i] = buffer[read_bytes - i - 1];
            buffer[read_bytes - i - 1] = temp;
        }

        fwrite(buffer, sizeof(char), read_bytes, out);
        offset -= BLOCK_SIZE;
    }

    fclose(in);
    fclose(out);
}

double measure_time(const char* input_file, const char* output_file, void (*func)(FILE*, FILE*)) {
    clock_t start, end;
    double cpu_time_used;

    FILE *in, *out;
    open_files(input_file, output_file, &in, &out);

    start = clock();
    func(in, out);
    end = clock();

    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    return cpu_time_used;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input file> <output file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *input_file = argv[1];
    const char *output_file = argv[2];

#ifdef REVERSE_BYTE
    double time1 = measure_time(input_file, output_file, reverse_byte);
    printf("\nExecution time for one byte copying: %lf \n", time1);
#elif REVERSE_BLOCK
    double time2 = measure_time(input_file, output_file, reverse_block);
    printf("\nExecution time for block copying: %lf \n", time2);
#else
    double time1 = measure_time(input_file, output_file, reverse_byte);
    printf("\nExecution time for one byte copying: %lf \n", time1);
    double time2 = measure_time(input_file, output_file, reverse_block);
    printf("Execution time for block copying: %lf \n", time2);
#endif

    return 0;
}
