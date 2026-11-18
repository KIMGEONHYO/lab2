#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    int fd[2];
    pid_t pid;
    const char message[] = "부모 프로세스에서 자식 프로세스로 전달한 메시지";
    char buffer[128] = {0};

    if (pipe(fd) == -1) {
        perror("pipe");
        return 1;
    }

    pid = fork();
    if (pid == -1) {
        perror("fork");
        close(fd[0]);
        close(fd[1]);
        return 1;
    }

    if (pid == 0) {
        close(fd[1]);  // 자식은 파이프에서 읽기만 함

        ssize_t bytes_read = read(fd[0], buffer, sizeof(buffer) - 1);
        if (bytes_read == -1) {
            perror("read");
            close(fd[0]);
            return 1;
        }

        buffer[bytes_read] = '\0';
        printf("자식 프로세스(PID=%ld)가 받은 메시지: %s\n",
               (long)getpid(), buffer);

        close(fd[0]);
        return 0;
    }

    close(fd[0]);  // 부모는 파이프에 쓰기만 함

    size_t message_length = strlen(message);
    ssize_t bytes_written = write(fd[1], message, message_length);
    if (bytes_written == -1) {
        perror("write");
        close(fd[1]);
        waitpid(pid, NULL, 0);
        return 1;
    }

    printf("부모 프로세스(PID=%ld)가 메시지를 보냈습니다.\n",
           (long)getpid());

    close(fd[1]);
    waitpid(pid, NULL, 0);

    return 0;
}
