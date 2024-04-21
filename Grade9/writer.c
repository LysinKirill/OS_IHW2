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
        msg.mtype = rand() % ARRAY_SIZE + 1; // Assuming index starts from 1
        msg.mdata = rand() % 25; // New value to be written

        if (msgsnd(msg_id, &msg, sizeof(msg.mdata), 0) == -1) {
            perror("msgsnd");
            exit(EXIT_FAILURE);
        }

        printf("Writer wrote: Changed value at index %ld to %d\n", msg.mtype, msg.mdata);
        sleep((rand() % 5) + 1); // Random delay between 1 and 5 seconds
    }

    return 0;
}
