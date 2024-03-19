#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

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

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    DIR *input_dir, *output_dir;

    struct dirent *entry;
    struct stat file_stat;

    input_dir = opendir(argv[1]);
    if (input_dir == NULL) {
        perror("Input directory cannot be opened");
        return 1;
    }

    output_dir = opendir(argv[2]);
    if (output_dir == NULL) {
        if (mkdir(argv[2], 0700) == -1) {
            perror("Cannot create output directory");
            return 1;
        } else {
            output_dir = opendir(argv[2]);
            if (output_dir == NULL) {
                perror("Output directory cannot be opened");
                return 1;
            }
        }
        output_dir = opendir(argv[2]);
    }

    while ((entry = readdir(input_dir)) != NULL) {
        if (stat(entry->d_name, &file_stat) == -1) {
            perror("Cannot get file information");
            continue;
        }

        char *ext = strstr(entry->d_name, ".txt");
        if (ext == NULL) {
            printf("File: %s not a .txt file\n", entry->d_name);
            continue;
        }

        char input_file_path[1024];
        snprintf(input_file_path, sizeof(input_file_path), "%s/%s", argv[1], entry->d_name);

        FILE *input_file = fopen(input_file_path, "r");
        if (input_file == NULL) {
            perror("Cannot open input file");
            continue;
        }

        char output_file_path[1024];
        snprintf(output_file_path, sizeof(output_file_path), "%s/%s", argv[2], entry->d_name);

        FILE *output_file = fopen(output_file_path, "w");
        if (output_file == NULL) {
            perror("Cannot open output file");
            fclose(input_file);
            continue;
        }

        reverse_byte(input_file, output_file);

        fclose(input_file);
        fclose(output_file);

    }


    closedir(input_dir);
    closedir(output_dir);

    return 0;
}