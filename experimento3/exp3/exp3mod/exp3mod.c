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

#define PROTECT

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

//////////////////// Constants ////////////////////

#define PRODUCER_SEM_KEY 9014
#define CONSUMER_SEM_KEY 9015
#define BUFFER_SEM_KEY 9016

#define PRODUCER_SHM_KEY 9017
#define CONSUMER_SHM_KEY 9018
#define BUFFER_SHM_KEY 9019

#define NO_OF_CHILDREN 8
#define SEM_PERMS 0666

//////////////////// Global Variables ////////////////////

char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz 1234567890";

int	producer_semaphore_id;
int	consumer_semaphore_id;
int	buffer_semaphore_id;

int shared_mem_producer_index_id;
int *shared_mem_producer_index_address;
int shared_mem_consumer_index_id;
int *shared_mem_consumer_index_address;
int shared_mem_buffer_id;
char *shared_mem_buffer_address;

struct sembuf semaphore_lock_op[1];
struct sembuf semaphore_unlock_op[1];

//////////////////// Functions ////////////////////

void produceCharacters(int child_number);
void consumeCharacters();
void createSemaphore(int *semaphore_id, int semaphore_key);
void createIntSharedMemory(int *shared_mem_id, int shared_mem_key, int **shared_mem_address);
void createCharSharedMemory(int *shared_mem_id, int shared_mem_key, char **shared_mem_address);
void removeSemaphore(int semaphore_id);
void removeSharedMemory(int shared_mem_id);

int main(int argc, char *argv[]) {
    int pid[NO_OF_CHILDREN];
    
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
        produceCharacters(child_number);
    } else if (rtn == 0 && (child_number % 2) != 0) {
        // Consumer Child
        fprintf(stdout, "Filho consumidor %d comecou...\n", child_number);
        consumeCharacters();
    } else {
        // Espera-se um determinado tempo em que os filhos irão imprimir os dados na tela
        // Após este tempo, mata-se os filhos, encerra-se o semáforo e memória compartilhada, e o programa termina.
        usleep(2000);
        
        // Matando os filhos
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

void produceCharacters(int child_number) {
    int number;
    int tmp_index;
    int i;
    srand((int)time(NULL));   // should only be called once
    // Este tempo permite que todos os filhos sejam iniciados
    usleep(3000);
    
    // Loop que fará com que os filhos imprimam o vetor de
    // caracteres até o timer de 15000micro seg do pai se esgotar
    while (1) {
        // Generates a random numerb between 1 and 5
        number = (rand() % 5) + 1;
        
#ifdef PROTECT
        if (semop(producer_semaphore_id, semaphore_lock_op, 1) == -1) {
            fprintf(stderr, "chamada semop() falhou, impossivel fechar o recurso! Erro: %s", strerror(errno));
            exit(1);
        }
#endif
        
        /*
         * Lendo o indice do segmento de memoria compartilhada
         */
        tmp_index = *shared_mem_producer_index_address;
        
        /*
         * Repita o numero especificado de vezes, esteja certo de nao
         * ultrapassar os limites do vetor, o comando if garante isso
         */
        for (i = 0; i < number; ++i) {
            if (!(tmp_index + i >= sizeof(alphabet))) {
                shared_mem_buffer_address[tmp_index + i] = alphabet[tmp_index + i];
                fprintf(stdout, "%c", alphabet[tmp_index + i]);
                fflush(stdout);
                usleep(number);
            } else {
                break;
            }
        }
        
        *shared_mem_producer_index_address = tmp_index + i;
        
        /*
         * Se o indice e maior que o tamanho do alfabeto, exibe um
         * caractere return para iniciar a linha seguinte e coloca
         * zero no indice
         */
        if (*shared_mem_producer_index_address >= sizeof(alphabet)) {
            *shared_mem_buffer_address = '\n';
            shared_mem_buffer_address -= (sizeof(alphabet) * sizeof(char));
            int j;
            int printSize = sizeof(alphabet);
            fprintf(stdout, "\n\n Finalizando: ");
            fflush(stdout);
            for (j = 0; j <= printSize; ++j) {
                fprintf(stdout, "%s", (char *)shared_mem_buffer_address + (j * sizeof(char)));
                fflush(stdout);
            }
            fprintf(stdout, "\n\n");
            fflush(stdout);
            *shared_mem_producer_index_address = 0;
        }
        
#ifdef PROTECT
        if (semop(producer_semaphore_id, semaphore_unlock_op, 1) == -1) {
            fprintf(stdout, "chamada semop() falhou, impossivel liberar o recurso!");
            exit(1);
        }
#endif
    }
}

void consumeCharacters() {
    
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
    // Removendo o semaforo
    if (semctl(semaphore_id, 0, IPC_RMID, 0) != 0) {
        fprintf(stderr, "Impossivel remover o conjunto de semaforos! Error: %s\n semaphore id: %d", strerror(errno), semaphore_id);
        exit(1);
    } else {
        fprintf(stdout, "\nConjunto de semaforos removido com sucesso!");
    }
}

void removeSharedMemory(int shared_mem_id) {
    // Removendo a memoria compartilhada
    if (shmctl(shared_mem_id, IPC_RMID, NULL) != 0) {
        fprintf(stderr, "Impossivel remover o segmento de memoria compartilhada!\n");
        exit(1);
    } else {
        fprintf(stdout, "\nSegmento de memoria compartilhada removido com sucesso!");
    }
}
