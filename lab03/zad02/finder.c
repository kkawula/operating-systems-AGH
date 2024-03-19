#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>

int main() {
    DIR *dir;
    struct dirent *entry;
    struct stat file_stat;
    long long total_size = 0;

    dir = opendir(".");
    if (dir == NULL) {
        perror("Directory cannot be opened");
        return 1;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (stat(entry->d_name, &file_stat) == -1) {
            perror("Cannot get file information");
            continue;
        }

        if (!S_ISDIR(file_stat.st_mode)) {
            printf("File: %s, Size: %lld bytes\n", entry->d_name, (long long)file_stat.st_size);
            total_size += file_stat.st_size;
        }
    }

    printf("Total size: %lld bytes\n", total_size);

    closedir(dir);

    return 0;
}