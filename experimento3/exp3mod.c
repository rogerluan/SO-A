//
//  exp3mod.c
//
//  Created on Apr 2nd 2017
//
//  ALEX VENTURINI  15294739
//  BRUNO PEDROSO   12662136
//  LUAN BONOMI     15108780
//  PEDRO CATALINI  15248354
//  ROGER OBA       12048534
//

//#define PROTECT
//#define LOG_DEBUG

/*
 * Includes Necessarios
 */
#include <errno.h>              /* errno and error codes */
#include <stdlib.h>             /* exit() */
#include <sys/time.h>           /* for gettimeofday() */
#include <stdio.h>              /* for printf() */
#include <unistd.h>             /* for fork() */
#include <sys/wait.h>           /* for wait() */
#include <signal.h>             /* for kill(), sigsuspend(), others */
#include <sys/ipc.h>            /* for all IPC function calls */
#include <sys/shm.h>            /* for shmget(), shmat(), shmctl() */
#include <sys/sem.h>            /* for semget(), semop(), semctl() */
#include <string.h>             /* for strerror() */

#ifndef MIN
#define MIN(a, b) (((a) > (b) ? (a) : (b)))
#endif

//////////////////// Constants ////////////////////

#define NO_OF_CHILDREN 8
#define SEM_PERMS 0666
#define BUFFER_SIZE 64

#define PRODUCER_SEM_KEY 9014
#define CONSUMER_SEM_KEY 9015
#define BUFFER_SEM_KEY 9016

#define PRODUCER_SHM_KEY 9017
#define CONSUMER_SHM_KEY 9018
#define BUFFER_SHM_KEY 9019

//////////////////// Global Variables ////////////////////

char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz 1234567890";

int	producer_semaphore_id; // Stores the id of the producer semaphore
int	consumer_semaphore_id; // Stores the id of the consumer semaphore
int	buffer_semaphore_id; // Stores the id of the buffer semaphore

int shared_mem_producer_index_id; // Stores the id of the producerindex shared memory
int *shared_mem_producer_index_address; // Pointer to the address where it's stored the current index of the producer
int shared_mem_consumer_index_id; // Stores the id of the consumer index shared memory
int *shared_mem_consumer_index_address; // Pointer to the address where it's stored the current index of the consumer
int shared_mem_buffer_id; // Stores the id of the buffer shared memory resource
char *shared_mem_buffer_address; // Pointer to the address where it's stored the beginning of the char buffer

struct sembuf semaphore_lock_op[1]; // Struct containing a lock operation
struct sembuf semaphore_unlock_op[1]; // Struct containing an unlock operation

//////////////////// Functions ////////////////////

/**
 Reads 1-5 characters from the alphabet vector and loads them into the buffer.
 Once the buffer limit is reached, prints it.
 */
void produceCharacters();

/**
 Reads the number of characters available to be read in the buffer (limited to 5)
 and replace them with the `#` character.
 Once the buffer limit is reached, prints it.
 */
void consumeCharacters();

/**
 Prints the content of the buffer.
 */
void printBuffer();

/**
 Locks a semaphore.

 @param semaphore_id id of the semaphore to be locked.
 */
void lockSemaphore(int semaphore_id);

/**
 Unlocks a semaphore.

 @param semaphore_id id of the semaphore to be unlocked.
 */
void unlockSemaphore(int semaphore_id);

/**
 Creates a semaphore.

 @param semaphore_id pointer to the semaphore id that will be returned from the semaphore creation function.
 @param semaphore_key key to be used to generate (or fetch) the semaphore.
 */
void createSemaphore(int *semaphore_id, int semaphore_key);

/**
 Creates a shared memory that points to an integer.

 @param shared_mem_id pointer to the shared memory id that will be returned from the shared memory creation function.
 @param shared_mem_key key to be used to generate (or fetch) the shared memory.
 @param shared_mem_address pointer to the shared memory address pointer attached to the generated (or fetched) shared memory
 */
void createIntSharedMemory(int *shared_mem_id, int shared_mem_key, int **shared_mem_address);

/**
 Creates a shared memory that points to a char.
 
 @param shared_mem_id pointer to the shared memory id that will be returned from the shared memory creation function.
 @param shared_mem_key key to be used to generate (or fetch) the shared memory.
 @param shared_mem_address pointer to the shared memory address pointer attached to the generated (or fetched) shared memory
 */
void createCharSharedMemory(int *shared_mem_id, int shared_mem_key, char **shared_mem_address);

/**
 Marks a semaphore to be removed.

 @param semaphore_id id of the semaphore to be marked to be removed.
 */
void removeSemaphore(int semaphore_id);

/**
 Marks a shared memory to be removed.
 
 @param shared_mem_id id of the shared memory to be marked to be removed.
 */
void removeSharedMemory(int shared_mem_id);

int main(int argc, char *argv[]) {
    int pid[NO_OF_CHILDREN];
    srand((int)time(NULL));   // should only be called once
    
    // Initializing the semaphore operators
    semaphore_lock_op[0].sem_num = 0;
    semaphore_lock_op[0].sem_op = -1;
    semaphore_lock_op[0].sem_flg = 0;
    semaphore_unlock_op[0].sem_num = 0;
    semaphore_unlock_op[0].sem_op = 1;
    semaphore_unlock_op[0].sem_flg = 0;
    
    createSemaphore(&producer_semaphore_id, PRODUCER_SEM_KEY);
    createSemaphore(&consumer_semaphore_id, CONSUMER_SEM_KEY);
    createSemaphore(&buffer_semaphore_id, BUFFER_SEM_KEY);
    
    createIntSharedMemory(&shared_mem_producer_index_id, PRODUCER_SHM_KEY, &shared_mem_producer_index_address);
    createIntSharedMemory(&shared_mem_consumer_index_id, CONSUMER_SHM_KEY, &shared_mem_consumer_index_address);
    createCharSharedMemory(&shared_mem_buffer_id, BUFFER_SHM_KEY, &shared_mem_buffer_address);
    
    int rtn = 1;
    int child_number;
    for (child_number = 0; child_number < NO_OF_CHILDREN; ++child_number) {
        if (rtn != 0) {
            rtn = fork();
            pid[child_number] = rtn;
        } else {
            break;
        }
    }
    
    if (rtn == 0 && (child_number % 2) == 0) {
        // Producer Child
        fprintf(stdout, "Filho produtor %d comecou...\n", child_number);
        fflush(stdout);
        produceCharacters();
    } else if (rtn == 0 && (child_number % 2) != 0) {
        // Consumer Child
        fprintf(stdout, "Filho consumidor %d comecou...\n", child_number);
        fflush(stdout);
        consumeCharacters();
    } else {
        // Sleeps for a certain amount of time so the children can do their jobs.
        // After this time, kills the children, removes the semaphores and shared memories and exits the program.
        usleep(90000);
        
        // Killing the children
        int i;
        for (i = 0; i < NO_OF_CHILDREN; ++i) {
            kill(pid[i], SIGKILL);
        }
        removeSemaphore(producer_semaphore_id);
        removeSemaphore(consumer_semaphore_id);
        removeSemaphore(buffer_semaphore_id);
        
        removeSharedMemory(shared_mem_producer_index_id);
        removeSharedMemory(shared_mem_consumer_index_id);
        removeSharedMemory(shared_mem_buffer_id);
        
        exit(0);
    }
}

void produceCharacters() {
    int number;
    int temp_index;
    int i;
    // This sleep time allows the children to be initialied.
    usleep(4000);
    // Loop that will make the children do their job until the parent process kills them
    while (1) {
        // Generates a random number between 1 and 5
        number = (rand() % 5) + 1;
#ifdef LOG_DEBUG
        fprintf(stdout, "\n\n<random number: %d>", number);
        fflush(stdout);
#endif
        lockSemaphore(producer_semaphore_id);
        
        temp_index = *shared_mem_producer_index_address % BUFFER_SIZE;
#ifdef LOG_DEBUG
        fprintf(stdout, "\n<Temp Index: %d>, <producer index: %d>\n", temp_index, *shared_mem_producer_index_address);
        fflush(stdout);
#endif
        lockSemaphore(buffer_semaphore_id);
        for (i = 0; i < number; ++i) {
            if ((temp_index + i < sizeof(alphabet)) && temp_index + i < BUFFER_SIZE) {
                shared_mem_buffer_address[temp_index + i] = alphabet[temp_index + i];
                //fprintf(stdout, "Buffer[%d] = %c\n", temp_index + i, shared_mem_buffer_address[temp_index + i]);
                fprintf(stdout, "%c", shared_mem_buffer_address[temp_index + i]);
                fflush(stdout);
                usleep(number);
            } else {
                break;
            }
        }
        
#ifdef LOG_DEBUG
        fprintf(stdout, "<Generated i: %d, producer_index: %d>", i, *shared_mem_producer_index_address);
        fflush(stdout);
#endif
        *shared_mem_producer_index_address += i;
        unlockSemaphore(producer_semaphore_id);
        if ((temp_index + i) > BUFFER_SIZE-1) {
            shared_mem_buffer_address[BUFFER_SIZE] = '\n';
            fprintf(stdout, "\n\nFinalizando Produtor:\t");
            fflush(stdout);
            printBuffer();
        }
        unlockSemaphore(buffer_semaphore_id);
    }
}

void consumeCharacters() {
    int number;
    int temp_index;
    int i;
    
    usleep(4000);
    
    while (1) {
        lockSemaphore(consumer_semaphore_id);
        //lockSemaphore(producer_semaphore_id); // This can be used to produce more reliable data, but can be commented to increase performance of the consumer
        // Generates a random numerb between 1 and 5
        number = (rand() % 5) + 1;
        number = MIN(number, ((*shared_mem_producer_index_address) - (*shared_mem_consumer_index_address)));
        temp_index = *shared_mem_consumer_index_address % BUFFER_SIZE;
        
        lockSemaphore(buffer_semaphore_id);
        for (i = 0; i < number; ++i) {
            // What we're reading/consuming (setting as `#`) must be <= than what has been produced so far.
            if (temp_index + i <= *shared_mem_producer_index_address) {
                shared_mem_buffer_address[temp_index + i] = '#';
                usleep(number);
            } else {
                break;
            }
        }
        
        *shared_mem_consumer_index_address += temp_index + i;
        
        // If reached the end of the buffer
        if (temp_index + i > BUFFER_SIZE-1) {
            fprintf(stdout, "\n\nFinalizando Consumidor:\t");
            fflush(stdout);
            printBuffer();
        }
        unlockSemaphore(buffer_semaphore_id);
        //unlockSemaphore(producer_semaphore_id); // Comment this line if the producer semaphore lock is commented above
        unlockSemaphore(consumer_semaphore_id);
    }
}

void printBuffer() {
    // We chose not to lock and unlock the semaphore inside this method due to the lack of flexibility
    int j;
    for (j = 0; j < BUFFER_SIZE; ++j) {
        fprintf(stdout, "%c", shared_mem_buffer_address[j]);
        fflush(stdout);
    }
    fprintf(stdout, "\n");
    fflush(stdout);
}

void lockSemaphore(int semaphore_id) {
#ifdef PROTECT
    if (semop(semaphore_id, semaphore_lock_op, 1) == -1) {
        fprintf(stderr, "chamada semop() falhou, impossivel fechar o recurso! Erro: %s", strerror(errno));
        exit(1);
    } else {
        //fprintf(stdout, "\n<Locked semaphore with id: %d>\n", semaphore_id);
        //fflush(stdout);
    }
#endif
}

void unlockSemaphore(int semaphore_id) {
#ifdef PROTECT
    if (semop(semaphore_id, semaphore_unlock_op, 1) == -1) {
        fprintf(stdout, "chamada semop() falhou, impossivel liberar o recurso!");
        exit(1);
    } else {
        //fprintf(stdout, "\n<Unlocked semaphore with id: %d>\n", semaphore_id);
        //fflush(stdout);
    }
#endif
}

void createSemaphore(int *semaphore_id, int semaphore_key) {
    // Creating the semaphore
    if ((*semaphore_id = semget(semaphore_key, 1, IPC_CREAT | SEM_PERMS)) == -1) {
        fprintf(stderr, "chamada a semget() falhou, impossivel criar o conjunto de semaforos!");
        exit(1);
    }
    // Initializing it as unlocked
    if (semop(*semaphore_id, semaphore_unlock_op, 1) == -1) {
        fprintf(stderr, "chamada semop() falhou, impossivel inicializar o semaforo!");
        exit(1);
    }
}

void createIntSharedMemory(int *shared_mem_id, int shared_mem_key, int **shared_mem_address) {
    // Creating the shared memory segment
    if ((*shared_mem_id = shmget(shared_mem_key, sizeof(int), IPC_CREAT | SEM_PERMS)) == -1) {
        fprintf(stderr, "Impossivel criar o segmento de memoria compartilhada. Erro: %s\n", strerror(errno));
        exit(1);
    }
    if ((*shared_mem_address = (int *)shmat(*shared_mem_id, NULL, 0)) == (int *)-1) {
        fprintf(stderr, "Impossivel associar o segmento de memoria compartilhada! Erro: %s\n", strerror(errno));
        exit(1);
    }
    **shared_mem_address = 0;
}

void createCharSharedMemory(int *shared_mem_id, int shared_mem_key, char **shared_mem_address) {
    // Creating the shared memory segment
    if ((*shared_mem_id = shmget(shared_mem_key, sizeof(int), IPC_CREAT | SEM_PERMS)) == -1) {
        fprintf(stderr, "Impossivel criar o segmento de memoria compartilhada. Erro: %s\n", strerror(errno));
        exit(1);
    }
    if ((*shared_mem_address = (char *)shmat(*shared_mem_id, NULL, 0)) == (char *)-1) {
        fprintf(stderr, "Impossivel associar o segmento de memoria compartilhada! Erro: %s\n", strerror(errno));
        exit(1);
    }
    **shared_mem_address = 0;
}

void removeSemaphore(int semaphore_id) {
    if (semctl(semaphore_id, 0, IPC_RMID, 0) != 0) {
        fprintf(stderr, "Impossivel remover o conjunto de semaforos! Error: %s\n semaphore id: %d", strerror(errno), semaphore_id);
        exit(1);
    } else {
        fprintf(stdout, "\nConjunto de semaforos removido com sucesso!");
    }
}

void removeSharedMemory(int shared_mem_id) {
    if (shmctl(shared_mem_id, IPC_RMID, NULL) != 0) {
        fprintf(stderr, "Impossivel remover o segmento de memoria compartilhada!\n");
        exit(1);
    } else {
        fprintf(stdout, "\nSegmento de memoria compartilhada removido com sucesso!");
    }
}
