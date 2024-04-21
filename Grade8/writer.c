#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <signal.h>

#define SHARED_MEMORY_SIZE 1024
#define ARRAY_SIZE 10
#define KEY_FILE "./sync"

int *shared_memory;
int sem_id;

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

void sem_wait() {
    struct sembuf sb = {0, -1, 0};
    semop(sem_id, &sb, 1);
}

void sem_signal() {
    struct sembuf sb = {0, 1, 0};
    semop(sem_id, &sb, 1);
}

void cleanup() {
    // Detach shared memory
    shmdt(shared_memory);
}

void signal_handler(int signal) {
    printf("Received signal %d. Terminating program...\n", signal);
    
    // Cleanup resources
    cleanup();
    
    exit(EXIT_SUCCESS);
}

int main() {
    signal(SIGINT, signal_handler); // Register SIGINT handler

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

    sem_id = semget(key, 1, IPC_CREAT | 0666);
    if (sem_id == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    // Initialize semaphore value to 1
    union semun su;
    su.val = 1;
    if (semctl(sem_id, 0, SETVAL, su) == -1) {
        perror("semctl");
        exit(EXIT_FAILURE);
    }

    while (1) {
        sem_wait();
        int index = rand() % ARRAY_SIZE;
        int new_value = rand() % 25;
        int old_value = shared_memory[index];
        shared_memory[index] = new_value;
        printf("Writer wrote: Changed value at index %d from %d to %d\n", index, old_value, new_value);
        sem_signal();
        sleep((rand() % 5) + 1); // Random delay between 1 and 5 seconds
    }

    // Cleanup resources (This should never be reached)
    cleanup();
    return 0;
}