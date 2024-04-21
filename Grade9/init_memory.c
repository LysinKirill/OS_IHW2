#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MESSAGE_QUEUE_KEY "./sync"

struct message {
    long mtype;
    int mdata;
};

int main() {
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

    printf("Message queue initialized successfully.\n");

    return 0;
}
