//
//  exp5
//
//  Created on Apr 19th 2017
//
//  ALEX VENTURINI  15294739
//  BRUNO PEDROSO   12662136
//  LUAN BONOMI     15108780
//  PEDRO CATALINI  15248354
//  ROGER OBA       12048534
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <semaphore.h>
#include <sys/types.h>		/* for wait(), msgget(), msgctl() */		//faltando sys
#include <sys/wait.h>		/* for wait() */				//faltando sys
#include <sys/ipc.h>		/* for msgget(), msgctl() */			//faltando sys
#include <sys/msg.h>		/* for msgget(), msgctl() */			//faltando sys
#include <sys/shm.h>
#include <errno.h>
#include <sys/sem.h>

//********** define

#define Num_barbeiros 2
#define Num_clientes 20
#define CHAIRS 7                /* número de cadeiras para os clientes à espera */
#define TRUE 1
#define MESSAGE_QUEUE_ID 1024
#define SHM_KEY	9009
#define MESSAGE_MTYPE 1
#define MICRO_PER_SECOND 1000000
#define SEM_PERMS 0666
#define SEM_KEY_B 9016
#define SEM_KEY_C 9017
//********** semaforos

sem_t customers;                /* número de cliente à espera de atendimento */
sem_t barbers;                  /* número de barbeiros à espera de clientes */
sem_t mutex;                    /* para exclusão mútua */
int waiting = 0;                /* clientes que estão esperando (não estão cortando) */
//VARIAVEIS GLOBAIS
int semaphore_id_B;
int semaphore_id_C;
int shared_mem_chairs_id;
int *shared_mem_chairs_address;
struct sembuf semaphore_lock_op[1]; // Struct containing a lock operation
struct sembuf semaphore_unlock_op[1]; // Struct containing an unlock operation

/* protótipos */

void barber(int queue_id, int barbeiro);
void customer(int queue_id, int cliente);
void cut_hair(int Num_aleatory);
void customer_left(int cliente);
void apreciate_hair(int cliente, int barbeiro, float tempo, int numero_gerado);

void createIntSharedMemory(int *shared_mem_id, int shared_mem_key, int **shared_mem_address);
void createSemaphore(int *semaphore_id, int semaphore_key);
void removeSharedMemory(int shared_mem_id);
void removeSemaphore(int semaphore_id);
void lockSemaphore(int semaphore_id);
void unlockSemaphore(int semaphore_id);

//********* estruturas necessarias para o buffer

typedef struct {
    int valor_gerado;
    int numero_cliente;
    struct timeval send_time;
} data_t;

typedef struct {
    long mtype;
    data_t data;
} msgbuf_t;

//********* main
//memoria compartilhada
int	g_sem_id;
int	g_shm_id;
int	*g_shm_addr;

int main() {
    
    int queue_id;                   // declarando o ID da fila
    int rtn = 1;                    // declarando rtn
    int cont_barb = 0;              // contador de barbeiros
    int cont_cliente = 0;           // contador de clientes
    int controle = 0;
    // Initializing the semaphore operators
    semaphore_lock_op[0].sem_num = 0;
    semaphore_lock_op[0].sem_op = -1;
    semaphore_lock_op[0].sem_flg = 0;
    semaphore_unlock_op[0].sem_num = 0;
    semaphore_unlock_op[0].sem_op = 1;
    semaphore_unlock_op[0].sem_flg = 0;
    
    
    createSemaphore(&semaphore_id_B, SEM_KEY_B);
    createSemaphore(&semaphore_id_C, SEM_KEY_C);
    createIntSharedMemory(&shared_mem_chairs_id, SHM_KEY, &shared_mem_chairs_address);
    // criando fila de mensagens
    if( (queue_id = msgget(MESSAGE_QUEUE_ID, IPC_CREAT | 0666)) == -1 ) {
        printf("Impossivel criar a fila de mensagens!\n");
        exit(1);
    }
    
    
    // *********** criando os filhos barbeiros
    
    for(int i = 0; i<Num_barbeiros;i++){
        if(rtn > 0){        //pai cria filhos barbeiros
            cont_barb++;
            //printf("barbeiro %d criado\n", cont_barb);
            rtn = fork();
        }else{
            break;
        }
    }
    
    // *********** criando os filhos clientes
    
    if(rtn != 0){            // sou o pai, então crio os filhos clientes
        for(int i = 0; i<Num_clientes;i++){
            if(rtn > 0){
                cont_cliente++;
                //  printf("cliente %d criado\n", cont_cliente);
                controle++;
                rtn = fork();
            }else{
                break;
            }
        }
    }
    
    //********** atribuindo funções aos filhos
    
    for(int i = 1; i<=Num_barbeiros;i++){
        if(cont_barb == i && rtn == 0 && controle == 0){
            // printf("barbeiro %d iniciado\n", i);
            barber(queue_id, cont_barb);
        }
    }
    
    for(int i = 1; i<=Num_clientes;i++){
        if(cont_cliente == i && rtn == 0 && controle != 0){
            //  printf("cliente %d iniciado\n", i);
            customer(queue_id, cont_cliente);
        }
    }
    
    //********* pai aguarda filhos terminarem de executar
    
    if(rtn > 0){
        usleep(1000000);
        // *********** removendo memoria compartilhada
        removeSharedMemory(shared_mem_chairs_id);
        removeSemaphore(semaphore_id_B);
        removeSemaphore(semaphore_id_C);
        if (msgctl(queue_id, IPC_RMID, NULL) == -1) {
            printf("Impossivel remover a fila!\n");
        }
    }
    return 0;
}

//******************** BARBEIRO

void barber(int queue_id, int barbeiro) {
    usleep(1000);
    float tempo = 0.0;
    struct timeval receive_time;                            // variavel para armazenar o tempo atual
    msgbuf_t message_buffer;                                // Este e o buffer para as mensagens enviadas
   	int i=0;
    while(i<Num_clientes){
        if (msgrcv(queue_id, (struct msgbuf *)&message_buffer, sizeof(data_t), MESSAGE_MTYPE, 0) == -1) {
            //fprintf(stderr, "Impossivel receber mensagem!\n");
            exit(1);
        }
        lockSemaphore(semaphore_id_B);
        if(*shared_mem_chairs_address >0){
            *shared_mem_chairs_address = *shared_mem_chairs_address -1;
            unlockSemaphore(semaphore_id_B);
            printf("barbeiro #%i esta atendendo o cliente #%i, fila = %i\n", barbeiro, message_buffer.data.numero_cliente,*shared_mem_chairs_address);
            
            cut_hair(message_buffer.data.valor_gerado);
            
            // pega o tempo final
            gettimeofday(&receive_time, NULL);
            // subtrai tempo final do inicial
            tempo = ((receive_time.tv_usec) - (message_buffer.data.send_time.tv_usec))/(float)MICRO_PER_SECOND;
            
            // função que exibe todos os dados pedidos
            
            apreciate_hair(message_buffer.data.numero_cliente, barbeiro, (tempo/(float)MICRO_PER_SECOND), message_buffer.data.valor_gerado);
            
        }else{
            unlockSemaphore(semaphore_id_B);
            printf("barbeiro dormiu\n");
        }
    }
    return;
}


//****************** CLIENTE

void customer(int queue_id, int cliente) {
    usleep(1000);
    int valor_gerado;                                       // armazena o valor aleatorio entre 2-1023
    struct timeval send_time;                               // variavel para armazenar o tempo atual
    msgbuf_t message_buffer;                                // Este e o buffer para as mensagens enviadas
    
    srand(time(NULL) ^ (getpid()<<16));
    valor_gerado = (rand() % 1023) + 1;
    
    gettimeofday(&send_time, NULL);
    
    // atribui os dados na struct que sera enviada há fila
    message_buffer.mtype = MESSAGE_MTYPE;
    message_buffer.data.valor_gerado = valor_gerado;
    message_buffer.data.numero_cliente = cliente;
    message_buffer.data.send_time = send_time;
    lockSemaphore(semaphore_id_B);
    if(*shared_mem_chairs_address < 7){
        //printf("um cliente se sentou\n");
        *shared_mem_chairs_address = *shared_mem_chairs_address + 1 ;
        unlockSemaphore(semaphore_id_B);
        printf("cliente #%i se sentou cadeieras preenchidas =%i\n", cliente ,*shared_mem_chairs_address);
        if (msgsnd(queue_id, (struct msgbuf *)&message_buffer, sizeof(data_t), 0) == -1) {
            printf("Impossivel enviar mensagem!\n");
            exit(1);
        }
    }else{
        customer_left(cliente);
        unlockSemaphore(semaphore_id_B);
    }
    return;
}

// ***************** FUNÇOES EXTRAS
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
void removeSharedMemory(int shared_mem_id) {
    if (shmctl(shared_mem_id, IPC_RMID, NULL) != 0) {
        fprintf(stderr, "Impossivel remover o segmento de memoria compartilhada!\n");
        exit(1);
    } else {
        fprintf(stdout, "\nSegmento de memoria compartilhada removido com sucesso!");
    }
}
void removeSemaphore(int semaphore_id) {
    if (semctl(semaphore_id, 0, IPC_RMID, 0) != 0) {
        fprintf(stderr, "Impossivel remover o conjunto de semaforos! Error: %s\n semaphore id: %d", strerror(errno), semaphore_id);
        exit(1);
    } else {
        fprintf(stdout, "\nConjunto de semaforos removido com sucesso!");
    }
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

void cut_hair(int Num_aleatory) {
    int vet[Num_aleatory], aux, i, j, k;
    
    for(i = 0; i<Num_aleatory; ++i){
        vet[i] = rand();
    }
    for(i = 0; i < Num_aleatory; i++){
        for(j = 0; j < k; j++){
            if(vet[j] > vet[j+1]){
                aux = vet[j];
                vet[j] = vet[j+1];
                vet[j+1]=aux;
            }
        }
        k--;
    }
}

void customer_left(int cliente) {
    printf("cliente #%i foi embora\n", cliente);
}

void apreciate_hair(int cliente, int barbeiro, float tempo, int numero_gerado) {
    printf("tempo até o cliente #%d ser atendido pelo barbeiro #%d: %.10f\nO numero gerado e %d\n\n",
           cliente, barbeiro, tempo, numero_gerado);
}
