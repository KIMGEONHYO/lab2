#define _DEFAULT_SOURCE

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define PATH_BUFFER_SIZE 4096

static int is_dot_entry(const char *name)
{
    return strcmp(name, ".") == 0 || strcmp(name, "..") == 0;
}

static int list_directory(const char *path)
{
    struct dirent **entries;
    int count = scandir(path, &entries, NULL, alphasort);

    if (count == -1) {
        perror(path);
        return -1;
    }

    printf("%s:\n", path);

    for (int i = 0; i < count; i++) {
        if (!is_dot_entry(entries[i]->d_name)) {
            printf("  %s\n", entries[i]->d_name);
        }
    }

    int result = 0;

    for (int i = 0; i < count; i++) {
        if (is_dot_entry(entries[i]->d_name)) {
            continue;
        }

        char child_path[PATH_BUFFER_SIZE];
        int length = snprintf(child_path, sizeof(child_path),
                              "%s/%s", path, entries[i]->d_name);

        if (length < 0 || length >= (int)sizeof(child_path)) {
            fprintf(stderr, "경로가 너무 깁니다: %s/%s\n",
                    path, entries[i]->d_name);
            result = -1;
            continue;
        }

        struct stat info;
        if (lstat(child_path, &info) == -1) {
            perror(child_path);
            result = -1;
            continue;
        }

        if (S_ISDIR(info.st_mode)) {
            printf("\n");
            if (list_directory(child_path) == -1) {
                result = -1;
            }
        }
    }

    for (int i = 0; i < count; i++) {
        free(entries[i]);
    }
    free(entries);

    return result;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "사용법: %s 디렉터리\n", argv[0]);
        return EXIT_FAILURE;
    }

    struct stat info;
    if (lstat(argv[1], &info) == -1) {
        perror(argv[1]);
        return EXIT_FAILURE;
    }

    if (!S_ISDIR(info.st_mode)) {
        fprintf(stderr, "%s는 디렉터리가 아닙니다.\n", argv[1]);
        return EXIT_FAILURE;
    }

    return list_directory(argv[1]) == 0
               ? EXIT_SUCCESS
               : EXIT_FAILURE;
}
