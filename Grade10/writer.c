#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <signal.h>

#define MESSAGE_QUEUE_NAME "/sync"
#define ARRAY_SIZE 10

void signal_handler(int signal) {
    printf("Received signal %d. Terminating program...\n", signal);
    exit(EXIT_SUCCESS);
}

int main() {
    signal(SIGINT, signal_handler); // Register SIGINT handler

    mqd_t mq = mq_open(MESSAGE_QUEUE_NAME, O_WRONLY);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    int value;

    while (1) {
        value = rand() % 25; // New value to be written

        if (mq_send(mq, (char *)&value, sizeof(value), 0) == -1) {
            perror("mq_send");
            exit(EXIT_FAILURE);
        }

        printf("Writer wrote: Changed value to %d\n", value);
        sleep((rand() % 5) + 1); // Random delay between 1 and 5 seconds
    }

    mq_close(mq);

    return 0;
}
