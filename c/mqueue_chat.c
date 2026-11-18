#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
    char parent_queue_name[64];
    char child_queue_name[64];
    struct mq_attr attr = {0};
    mqd_t parent_to_child;
    mqd_t child_to_parent;
    pid_t pid;

    snprintf(parent_queue_name, sizeof(parent_queue_name),
             "/lab2_p2c_%ld", (long)getpid());
    snprintf(child_queue_name, sizeof(child_queue_name),
             "/lab2_c2p_%ld", (long)getpid());

    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 128;

    parent_to_child = mq_open(parent_queue_name,
                              O_CREAT | O_RDWR, 0600, &attr);
    if (parent_to_child == (mqd_t)-1) {
        perror("mq_open parent_to_child");
        return 1;
    }

    child_to_parent = mq_open(child_queue_name,
                              O_CREAT | O_RDWR, 0600, &attr);
    if (child_to_parent == (mqd_t)-1) {
        perror("mq_open child_to_parent");
        mq_close(parent_to_child);
        mq_unlink(parent_queue_name);
        return 1;
    }

    pid = fork();
    if (pid == -1) {
        perror("fork");
        mq_close(parent_to_child);
        mq_close(child_to_parent);
        mq_unlink(parent_queue_name);
        mq_unlink(child_queue_name);
        return 1;
    }

    if (pid == 0) {
        char buffer[128];

        if (mq_receive(parent_to_child, buffer, sizeof(buffer), NULL) == -1) {
            perror("child mq_receive");
            return 1;
        }
        printf("자식이 받은 메시지: %s\n", buffer);

        const char reply[] = "자식: 메시지 잘 받았어요.";
        if (mq_send(child_to_parent, reply, strlen(reply) + 1, 0) == -1) {
            perror("child mq_send");
            return 1;
        }

        mq_close(parent_to_child);
        mq_close(child_to_parent);
        return 0;
    }

    const char message[] = "부모: 안녕하세요!";
    if (mq_send(parent_to_child, message, strlen(message) + 1, 0) == -1) {
        perror("parent mq_send");
        return 1;
    }

    char buffer[128];
    if (mq_receive(child_to_parent, buffer, sizeof(buffer), NULL) == -1) {
        perror("parent mq_receive");
        return 1;
    }
    printf("부모가 받은 메시지: %s\n", buffer);

    waitpid(pid, NULL, 0);

    mq_close(parent_to_child);
    mq_close(child_to_parent);
    mq_unlink(parent_queue_name);
    mq_unlink(child_queue_name);

    return 0;
}
