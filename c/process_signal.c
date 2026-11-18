#define _POSIX_C_SOURCE 200809L

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static volatile sig_atomic_t signal_received = 0;

static void handle_signal(int signal_number)
{
    signal_received = signal_number;
}

int main(void)
{
    struct sigaction action = {0};
    action.sa_handler = handle_signal;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    if (sigaction(SIGUSR1, &action, NULL) == -1) {
        perror("sigaction");
        return EXIT_FAILURE;
    }

    printf("부모 프로세스 PID: %ld\n", (long)getpid());
    fflush(stdout);

    pid_t child_pid = fork();

    if (child_pid == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (child_pid == 0) {
        printf("자식 프로세스 PID: %ld\n", (long)getpid());
        printf("자식의 부모 PID: %ld\n", (long)getppid());
        printf("자식 프로세스에서 SIGUSR1 발생\n");
        fflush(stdout);

        if (raise(SIGUSR1) != 0) {
            perror("raise");
            exit(EXIT_FAILURE);
        }

        if (signal_received == SIGUSR1) {
            printf("자식에서 SIGUSR1 처리 완료\n");
        }

        exit(EXIT_SUCCESS);
    }

    int status;
    if (waitpid(child_pid, &status, 0) == -1) {
        perror("waitpid");
        return EXIT_FAILURE;
    }

    if (WIFEXITED(status)) {
        printf("자식 프로세스 종료 상태: %d\n", WEXITSTATUS(status));
    } else {
        printf("자식 프로세스가 정상 종료되지 않았습니다.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
