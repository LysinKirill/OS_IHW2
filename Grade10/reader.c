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

int fibonacci(int num) {
    if (num == 0)
        return 0;
    if (num == 1)
        return 1;

    return fibonacci(num - 1) + fibonacci(num - 2);
}

int main() {
    signal(SIGINT, signal_handler); // Register SIGINT handler

    mqd_t mq = mq_open(MESSAGE_QUEUE_NAME, O_RDONLY);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    int value;

    while (1) {
        if (mq_receive(mq, (char *)&value, sizeof(value), NULL) == -1) {
            perror("mq_receive");
            exit(EXIT_FAILURE);
        }

        int index = value % ARRAY_SIZE;
        int fib = fibonacci(value);
        printf("Reader read at index %d, value: %d, Fibonacci: %d\n", index, value, fib);
    }

    mq_close(mq);

    return 0;
}
