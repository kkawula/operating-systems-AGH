#include "zad1.h"
#include <stdio.h>

void read_file(const char *file_path, char *str)
{
    /* Wczytaj tekst pliku file_path do bufora str */
    /* Pamiętaj aby dodać do str znak zakończenia string-a '\0' */

    FILE *file = fopen(file_path, "r");

    size_t size = fread(str, sizeof(char), 255, file);
    str[size] = '\0';

    fclose(file);

}

void write_file(const char *file_path, char *str)
{
    /* Zapisz tekst z bufora str do pliku file_path */
    /* Zapisz tylko tyle bajtów ile potrzeba. */
    /* Bufor może być większy niż tekst w nim zawarty*/

    FILE *file = fopen(file_path, "w");

    fwrite(str, sizeof(char), strlen(str), file);
    fclose(file);

}

int main(int arc, char **argv)
{
    char buffer[256];
    read_file(IN_FILE_PATH, buffer);
    check_buffer(buffer);

    char response[] = "What's the problem?";
    write_file(OUT_FILE_PATH, response);
    check_file();
}
