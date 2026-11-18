#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(void)
{
    const char *directory = "file_demo_dir";
    const char *file_path = "file_demo_dir/sample.txt";

    if (mkdir(directory, 0755) == -1 && errno != EEXIST) {
        perror("mkdir");
        return EXIT_FAILURE;
    }

    FILE *file = fopen(file_path, "w");
    if (file == NULL) {
        perror("fopen for writing");
        return EXIT_FAILURE;
    }

    if (fputs("Lab2 file and directory practice\n", file) == EOF) {
        perror("fputs");
        fclose(file);
        return EXIT_FAILURE;
    }

    if (fclose(file) == EOF) {
        perror("fclose");
        return EXIT_FAILURE;
    }

    file = fopen(file_path, "r");
    if (file == NULL) {
        perror("fopen for reading");
        return EXIT_FAILURE;
    }

    printf("파일에 기록한 내용:\n");

    char line[256];
    while (fgets(line, sizeof(line), file) != NULL) {
        fputs(line, stdout);
    }

    if (ferror(file)) {
        perror("fgets");
        fclose(file);
        return EXIT_FAILURE;
    }

    if (fclose(file) == EOF) {
        perror("fclose");
        return EXIT_FAILURE;
    }

    DIR *dir = opendir(directory);
    if (dir == NULL) {
        perror("opendir");
        return EXIT_FAILURE;
    }

    printf("디렉터리 항목:\n");

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 &&
            strcmp(entry->d_name, "..") != 0) {
            printf("%s\n", entry->d_name);
        }
    }

    if (closedir(dir) == -1) {
        perror("closedir");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
