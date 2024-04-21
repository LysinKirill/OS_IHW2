#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>

#define MESSAGE_QUEUE_KEY "./sync"
#define ARRAY_SIZE 10

struct message {
    long mtype;
    int mdata;
};

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

    key_t key = ftok(MESSAGE_QUEUE_KEY, 'Z');
    if (key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    int msg_id = msgget(key, IPC_CREAT | 0666);
    if (msg_id == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    struct message msg;

    while (1) {
        if (msgrcv(msg_id, &msg, sizeof(msg.mdata), 0, 0) == -1) {
            perror("msgrcv");
            exit(EXIT_FAILURE);
        }

        int index = msg.mtype;
        int value = msg.mdata;
        int fib = fibonacci(value);
        printf("Reader read at index %d, value: %d, Fibonacci: %d\n", index, value, fib);
    }

    return 0;
}
