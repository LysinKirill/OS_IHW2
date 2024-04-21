#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#define SHARED_MEMORY_SIZE 1024
#define ARRAY_SIZE 10

int *shared_memory;
sem_t *mutex;
sem_t *writer_semaphore;

void cleanup() {
    sem_unlink("/mutex");
    sem_unlink("/writer_semaphore");
}

void signal_handler(int signal) {
    printf("Received signal %d. Terminating program...\n", signal);
    
    // Cleanup resources
    cleanup();
    
    exit(EXIT_SUCCESS);
}

void create_semaphores() {
    mutex = sem_open("/mutex", O_CREAT | O_EXCL, 0666, 1);
    writer_semaphore = sem_open("/writer_semaphore", O_CREAT | O_EXCL, 0666, 1);

    if (mutex == SEM_FAILED || writer_semaphore == SEM_FAILED) {
        if (errno != EEXIST) {
            perror("sem_open");
            cleanup();
            exit(EXIT_FAILURE);
        } else {
            cleanup();
            create_semaphores(); // Try again
        }
    }
}

int fibonacci(int num) {
    if (num == 0)
        return 0;
    if (num == 1)
        return 1;

    return fibonacci(num - 1) + fibonacci(num - 2);
}

void reader(int id) {
    while (1) {
        sem_wait(mutex);
        int index = rand() % ARRAY_SIZE;
        int value = shared_memory[index];
        int fib = fibonacci(value);
        printf("Reader %d read at index %d, value: %d, Fibonacci: %d\n", id, index, value, fib);
        sem_post(mutex);
        usleep((rand() % 4500000) + 500000); // Random delay between 0.5 and 5 seconds
    }
}

void writer(int id) {
    while (1) {
        sem_wait(writer_semaphore);
        sem_wait(mutex);
        int index = rand() % ARRAY_SIZE;
        int new_value = rand() % 25;
        int old_value = shared_memory[index];
        shared_memory[index] = new_value;
        printf("Writer %d wrote: Changed value at index %d from %d to %d\n", id, index, old_value, new_value);
        sem_post(mutex);
        sem_post(writer_semaphore);
        usleep((rand() % 4500000) + 500000); // Random delay between 0.5 and 5 seconds
    }
}

int main(int argc, char *argv[]) {
    signal(SIGINT, signal_handler); // Register SIGINT handler
    
    if (argc != 3) {
        printf("Usage: %s <num_readers> <num_writers>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int num_readers = atoi(argv[1]);
    int num_writers = atoi(argv[2]);

    int shm_id = shmget(IPC_PRIVATE, SHARED_MEMORY_SIZE, IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    shared_memory = (int *)shmat(shm_id, NULL, 0);
    if (shared_memory == (int *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < ARRAY_SIZE; ++i) {
        shared_memory[i] = i + 1;
    }

    create_semaphores();

    pid_t pid;

    for (int i = 0; i < num_writers + num_readers; ++i) {
        pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            if (i < num_writers) {
                writer(i);
            } else {
                reader(i - num_writers);
            }
            break;
        }
    }

    // Wait for all child processes to finish
    for (int i = 0; i < num_writers + num_readers; ++i) {
        wait(NULL);
    }

    // Cleanup resources
    shmdt(shared_memory);
    shmctl(shm_id, IPC_RMID, NULL);
    cleanup();

    return 0;
}
