#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int source_fd;
    int destination_fd;
    struct stat source_stat;
    struct stat destination_stat;
    void *source_map;
    void *destination_map;

    if (argc != 3) {
        fprintf(stderr, "사용법: %s 원본파일 복사파일\n", argv[0]);
        return 1;
    }

    source_fd = open(argv[1], O_RDONLY);
    if (source_fd == -1) {
        perror("원본 파일 열기");
        return 1;
    }

    if (fstat(source_fd, &source_stat) == -1) {
        perror("원본 파일 정보 확인");
        close(source_fd);
        return 1;
    }

    destination_fd = open(argv[2], O_RDWR | O_CREAT, 0644);
    if (destination_fd == -1) {
        perror("복사 파일 열기");
        close(source_fd);
        return 1;
    }

    if (fstat(destination_fd, &destination_stat) == -1) {
        perror("복사 파일 정보 확인");
        close(source_fd);
        close(destination_fd);
        return 1;
    }

    if (source_stat.st_dev == destination_stat.st_dev &&
        source_stat.st_ino == destination_stat.st_ino) {
        fprintf(stderr, "원본과 복사 파일은 서로 다른 파일이어야 합니다.\n");
        close(source_fd);
        close(destination_fd);
        return 1;
    }

    if (ftruncate(destination_fd, source_stat.st_size) == -1) {
        perror("복사 파일 크기 설정");
        close(source_fd);
        close(destination_fd);
        return 1;
    }

    if (source_stat.st_size == 0) {
        printf("빈 파일을 복사했습니다: %s -> %s\n", argv[1], argv[2]);
        close(source_fd);
        close(destination_fd);
        return 0;
    }

    source_map = mmap(NULL, source_stat.st_size, PROT_READ,
                      MAP_PRIVATE, source_fd, 0);
    if (source_map == MAP_FAILED) {
        perror("원본 파일 mmap");
        close(source_fd);
        close(destination_fd);
        return 1;
    }

    destination_map = mmap(NULL, source_stat.st_size, PROT_READ | PROT_WRITE,
                           MAP_SHARED, destination_fd, 0);
    if (destination_map == MAP_FAILED) {
        perror("복사 파일 mmap");
        munmap(source_map, source_stat.st_size);
        close(source_fd);
        close(destination_fd);
        return 1;
    }

    memcpy(destination_map, source_map, source_stat.st_size);

    if (msync(destination_map, source_stat.st_size, MS_SYNC) == -1) {
        perror("msync");
        munmap(source_map, source_stat.st_size);
        munmap(destination_map, source_stat.st_size);
        close(source_fd);
        close(destination_fd);
        return 1;
    }

    printf("파일을 복사했습니다: %s -> %s (%ld바이트)\n",
           argv[1], argv[2], (long)source_stat.st_size);

    munmap(source_map, source_stat.st_size);
    munmap(destination_map, source_stat.st_size);
    close(source_fd);
    close(destination_fd);

    return 0;
}
