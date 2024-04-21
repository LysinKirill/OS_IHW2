#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <mqueue.h>

#define MESSAGE_QUEUE_NAME "/sync"
#define ARRAY_SIZE 10

int main() {
    mqd_t mq;

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10; // Maximum number of messages in the queue
    attr.mq_msgsize = sizeof(int);
    attr.mq_curmsgs = 0;

    mq = mq_open(MESSAGE_QUEUE_NAME, O_CREAT | O_RDWR, 0666, &attr);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    printf("Message queue initialized successfully.\n");

    mq_close(mq);

    return 0;
}
