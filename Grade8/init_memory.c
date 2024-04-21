#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHARED_MEMORY_SIZE 1024
#define ARRAY_SIZE 10
#define KEY_FILE "./sync"

int *shared_memory;

void cleanup() {
    shmdt(shared_memory);
}

int main() {
    key_t key = ftok(KEY_FILE, 'Z');
    if (key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    int shm_id = shmget(key, SHARED_MEMORY_SIZE, IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    shared_memory = (int *)shmat(shm_id, NULL, 0);
    if (shared_memory == (int *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    // Initialize shared memory with increasing values starting from 1
    for (int i = 0; i < ARRAY_SIZE; i++) {
        shared_memory[i] = i + 1;
    }

    printf("Shared memory initialized with increasing values starting from 1.\n");

    // Cleanup resources
    cleanup();
    return 0;
}