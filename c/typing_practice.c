#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define INPUT_SIZE 256

static size_t count_errors(const char *target, const char *typed)
{
    size_t target_length = strlen(target);
    size_t typed_length = strlen(typed);
    size_t compare_length =
        target_length > typed_length ? target_length : typed_length;
    size_t errors = 0;

    for (size_t i = 0; i < compare_length; i++) {
        if (i >= target_length || i >= typed_length ||
            target[i] != typed[i]) {
            errors++;
        }
    }

    return errors;
}

static double elapsed_seconds(struct timespec start, struct timespec end)
{
    return (double)(end.tv_sec - start.tv_sec) +
           (double)(end.tv_nsec - start.tv_nsec) / 1000000000.0;
}

int main(void)
{
    const char *sentences[] = {
        "Linux programs use files to store data.",
        "Processes can communicate through pipes.",
        "Rust vectors can store matrix elements."
    };
    const size_t sentence_count =
        sizeof(sentences) / sizeof(sentences[0]);

    char input[INPUT_SIZE];
    size_t total_errors = 0;
    size_t total_characters = 0;
    struct timespec start, end;

    printf("아래 문장을 보고 한 문장씩 입력하세요.\n");

    if (clock_gettime(CLOCK_MONOTONIC, &start) == -1) {
        perror("clock_gettime");
        return EXIT_FAILURE;
    }

    for (size_t i = 0; i < sentence_count; i++) {
        printf("\n문장 %zu/%zu\n", i + 1, sentence_count);
        printf("원문: %s\n", sentences[i]);
        printf("입력: ");

        if (fgets(input, sizeof(input), stdin) == NULL) {
            fprintf(stderr, "문장을 입력하지 못했습니다.\n");
            return EXIT_FAILURE;
        }

        input[strcspn(input, "\r\n")] = '\0';

        size_t errors = count_errors(sentences[i], input);
        size_t typed_length = strlen(input);

        total_errors += errors;
        total_characters += typed_length;

        printf("이 문장의 오타 수: %zu\n", errors);
    }

    if (clock_gettime(CLOCK_MONOTONIC, &end) == -1) {
        perror("clock_gettime");
        return EXIT_FAILURE;
    }

    double seconds = elapsed_seconds(start, end);
    if (seconds <= 0.0) {
        fprintf(stderr, "측정된 시간이 올바르지 않습니다.\n");
        return EXIT_FAILURE;
    }

    double characters_per_minute =
        (double)total_characters * 60.0 / seconds;

    printf("\n전체 오타 수: %zu\n", total_errors);
    printf("총 입력 문자 수: %zu\n", total_characters);
    printf("총 소요 시간: %.2f초\n", seconds);
    printf("평균 분당 입력 문자 수: %.2f자/분\n",
           characters_per_minute);

    return EXIT_SUCCESS;
}
