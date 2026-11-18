#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    int parent_to_child[2];
    int child_to_parent[2];
    pid_t pid;
    char buffer[128];

    if (pipe(parent_to_child) == -1 || pipe(child_to_parent) == -1) {
        perror("pipe");
        return 1;
    }

    pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        close(parent_to_child[1]);
        close(child_to_parent[0]);

        ssize_t n = read(parent_to_child[0], buffer, sizeof(buffer) - 1);
        if (n == -1) {
            perror("child read");
            return 1;
        }

        buffer[n] = '\0';
        printf("자식이 받은 메시지: %s\n", buffer);

        const char reply[] = "자식: 메시지 잘 받았어요.";
        if (write(child_to_parent[1], reply, strlen(reply)) == -1) {
            perror("child write");
            return 1;
        }

        close(parent_to_child[0]);
        close(child_to_parent[1]);
        return 0;
    }

    close(parent_to_child[0]);
    close(child_to_parent[1]);

    const char message[] = "부모: 안녕하세요!";
    if (write(parent_to_child[1], message, strlen(message)) == -1) {
        perror("parent write");
        return 1;
    }

    ssize_t n = read(child_to_parent[0], buffer, sizeof(buffer) - 1);
    if (n == -1) {
        perror("parent read");
        return 1;
    }

    buffer[n] = '\0';
    printf("부모가 받은 메시지: %s\n", buffer);

    close(parent_to_child[1]);
    close(child_to_parent[0]);
    waitpid(pid, NULL, 0);

    return 0;
}
