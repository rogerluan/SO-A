//
//  exp2.c
//
//  Created on 13/03/17
//
//  ALEX VENTURINI  15294739
//  BRUNO PEDROSO   12662136
//  LUAN BONOMI     15108780
//  PEDRO CATALINI  15248354
//  ROGER OBA       12048534
//

/*******************************************************************************
 *
 * Este programa faz parte do curso sobre tempo real do Laboratorio Embry-Riddle
 *
 * Seguem os comentarios originais:
 *
 * Experiment #3: Shared Resources, Measureing Message Queue Transfer Time
 *
 *    Programmer: Eric Sorton
 *          Date: 2/11/97
 *           For: MSE599, Special Topics Class
 *
 *       Purpose: The purpose of this program is to measure the time it takes
 *                a message to be transfered across a message queue.  The
 *                total time will include the time to make the call to msgsnd(),
 *                the time for the system to transfer the message, the time
 *                for the context switch, and finally, the time for the other
 *                end to call msgrcv().
 *
 *                The algorithm for this program is very simple:
 *
 *                   o The parent creates the message queue
 *                   o The parents starts two children
 *                   o The first child will:
 *                         - Receive a message on the queue
 *                         - Call gettimeofday() to get the current time
 *                         - Using the time in the message, calculate
 *                              the difference and store it in an array
 *                         - Loop (X number of times)
 *	   			  - Display the results
 *                   o The second child will:
 *                         - Call gettimeofday() to get the current time
 *                         - Place the time in a message
 *                         - Place the message on the queue
 *                         - Pause to allow the other child to run
 *                         - Loop (X number of times)
 *                   o The parent waits for the children to finish
 *
 * Traduzindo:
 *
 *     PropÛsito: O propÛsito deste programa È a medicao do tempo que leva
 *                uma mensagem para ser transferida por uma fila de mensagens.
 *                O tempo total incluira o tempo para realizar a chamada
 *                msgsnd(), o tempo para o sistema transferir a mensagem, o
 *                tempo para troca de contexto e, finalmente, o tempo para,
 *                na outra ponta, ocorrer a chamada msgrcv().
 *
 *                O algoritmo para este programa e bastante simples:
 *
 *                   o O pai cria a fila de mensagens
 *                   o O pai inicializa dois filhos
 *                   o O primeiro filho:
 *                         - Recebe uma mensagem pela fila
 *                         - Chama gettimeofday() para obter o tempo atual
 *                         - Usando o tempo existente na mensagem, calcula
 *                              a diferenca
 *                         - Repete (numero X de vezes)
 *				  - Exibe os resultados
 *                   o O segundo filho:
 *                         - Chama gettimeofday() para obter o tempo atual
 *                         - Coloca o tempo em uma mensagem
 *                         - Coloca a mensagem na fila
 *                         - Realiza uma pausa para permitir a execucao do irmao
 *                         - Repete (numero X de vezes)
 *                   o O pai espera os filhos terminarem
 *
 *******************************************************************************/

/*
 * Includes Necessarios
 */
#include <sys/time.h>   // gettimeofday()
#include <stdio.h>		// printf()
#include <stdlib.h>     // exit()
#include <unistd.h>		// fork()
#include <sys/types.h>		/* for wait(), msgget(), msgctl() */	//faltando sys
#include <sys/wait.h>		/* for wait() */                        //faltando sys
#include <sys/ipc.h>		/* for msgget(), msgctl() */			//faltando sys
#include <sys/msg.h>		/* for msgget(), msgctl() */			//faltando sys
#include <stdbool.h>
#include <float.h>

/*
 * NO_OF_ITERATIONS corresponde ao numero de mensagens que serao enviadas.
 * Se este numero cresce, o tempo de execucao tambem deve crescer.
 */
#define NO_OF_ITERATIONS	500

/*
 * MICRO_PER_SECOND define o numero de microsegundos em um segundo
 */
#define MICRO_PER_SECOND	1000000

/*
 * MESSAGE_QUEUE_ID eh uma chave arbitraria, foi escolhido um numero qualquer,
 * que deve ser unico. Se outra pessoa estiver executando este mesmo programa
 * ao mesmo tempo, o numero pode ter que ser mudado!
 */
#define MESSAGE_QUEUE_KEY1	3102
#define MESSAGE_QUEUE_KEY2	6204
#define SENDER_DELAY_TIME	10
#define MESSAGE_MTYPE_TIME  1
#define MESSAGE_MTYPE_DATA  2
#define NO_OF_CHILDREN 		2 // acrescentando numero de filhos

//********************** Data Structures **********************//

/**
 Data structure used to send the data over the message queue
 */
typedef struct {
    unsigned int msg_no;
    struct timeval send_time;
    float min;
    float max;
    float med;
    float total;
} data_t;

/**
 Data structure used to send a message over the message queue.
 */
typedef struct {
    long mtype;
    char mtext[sizeof(data_t)];
} msgbuf_t;

/**** Functions Declarations ****/

/**
 Sends the current time information to the message queue
 
 @param queue_id The messagr queue id to send the message to
 */
void sendTimeMessage(int queue_id);

/**
 Receives data from the message queue.

 @param queue_id message queue id
 @return returns the queue id of the temp message queue
 */
data_t *receiveTimeMessage(int queue_id);

/**
 Sends the calculated information to the message queue

 @param queue_id The message queue id to send the message to
 @param calculatedData The calculated data to be sent
 */
void sendCalculatedData(int queue_id, data_t *calculatedData);

/**
 Receives the calculated information from the message queue

 @param queue_id The message queue id to receive the message from
 */
void receiveCalculatedData(int queue_id);

/**
 Creates a new message queue with the given key. If the queue already exists, it simply returns the existing one.

 @param key Key to be used to create the message queue
 @return returns the queue id of the message queue created or fetched.
 */
int createOrGetQueueIdWithKey(key_t key);

/**
 Deletes the message queue with the given queue id

 @param queue_id The queue id to be deleted from the message queue
 */
void deleteQueueWithId(int queue_id);

int main(int argc, char *argv[]) {
    int rtn = 1;				//inicializa rtn
    int count = 0, i = 0;
    
    for (i = 0; i < NO_OF_CHILDREN; i++) {	//consertando o contador
        if (rtn > 0) {
            rtn = fork();
            count++;
        } else {
            break;
        }
    }
    
    if (rtn == 0 && count == 1) {
        // Cria a primeira fila de mensagens
        int first_queue_id = createOrGetQueueIdWithKey(MESSAGE_QUEUE_KEY1);
        printf("Receptor iniciado ...\n"); 		// fechando aspas
        data_t *calculatedData = receiveTimeMessage(first_queue_id);
        deleteQueueWithId(first_queue_id);
        
        pid_t pid = fork();
        if (pid == -1) {
            printf("Ocorreu um erro ao criar o terceiro filho.\n");
        } else if (pid == 0) {
            printf("Processo pai == segundo filho, cria a segunda fila e envia os dados\n");
            int second_queue_id = createOrGetQueueIdWithKey(MESSAGE_QUEUE_KEY2);
            sendCalculatedData(second_queue_id, calculatedData);
            exit(0);
        } else if (pid > 0) {
            printf("Processo filho == terceiro filho, recebe a mensagem e printa\n");
            int second_queue_id = createOrGetQueueIdWithKey(MESSAGE_QUEUE_KEY2);
            receiveCalculatedData(second_queue_id);
            wait(NULL);
            deleteQueueWithId(second_queue_id);
            exit(0);
        }
    } else if (rtn == 0 && count == 2) {		// faltando um = na comparacao
        printf("Emissor iniciado ...\n");
        int first_queue_id = createOrGetQueueIdWithKey(MESSAGE_QUEUE_KEY1);
        sendTimeMessage(first_queue_id);
        
        exit(0);
    } else {
        printf("Pai aguardando os filhos terminarem...\n");
        wait(NULL);
        wait(NULL);
        exit(0);
    }
}

data_t *receiveTimeMessage(int queue_id) {
    printf("Function - Receive Time Message with queue id: %d\n", queue_id);
    int count;
    struct timeval receive_time;
    float delta = 0;
    float max = 0;
    float min = 1000;
    float total = 0;
    
    msgbuf_t message_buffer;
    data_t *data_ptr = (data_t *)(message_buffer.mtext);
    
    for (count = 0; count < NO_OF_ITERATIONS; ++count) {
//        ssize_t msgrcvBytesRead = 0;
//        while (msgrcvBytesRead < sizeof(data_t)) {
//            ssize_t msgrcvReturnValue = msgrcv(queue_id, (struct msgbuf *)&message_buffer, sizeof(data_t), MESSAGE_MTYPE_TIME, 0);
//            if (msgrcvReturnValue == -1) {
//                fprintf(stderr, "A leitura da fila de mensagens retornou um erro, abortando.!\n");
//                exit(1);
//            } else {
//                msgrcvBytesRead += msgrcvReturnValue;
//            }
//        }
        if (msgrcv(queue_id, (struct msgbuf *)&message_buffer, sizeof(data_t), MESSAGE_MTYPE_TIME, 0) == -1) {
            fprintf(stderr, "Impossivel receber mensagem! receiveTimeMessage, count:%d \n", count);
            exit(1);
        }
        
        gettimeofday(&receive_time, NULL);
        
        delta = receive_time.tv_sec - data_ptr->send_time.tv_sec;
        delta += (receive_time.tv_usec - data_ptr->send_time.tv_usec) / (float)MICRO_PER_SECOND;
        total += delta;
        
        if (delta > max) {
            max = delta;
        }
        if (min > delta) {
            min = delta;
        }
    }
    printf("SENDER: O tempo medio de transferencia: \t%.12f\n", total / NO_OF_ITERATIONS);
    printf("SENDER: O tempo maximo de transferencia: \t%.12f\n", max);
    printf("SENDER: O tempo minimo de transferencia: \t%.12f\n", min);
    printf("SENDER: O tempo total de transferencia: \t%.12f\n", total);
    
    data_t *calculatedData;
    calculatedData->min = min;
    calculatedData->max = max;
    calculatedData->total = total;
    calculatedData->med = total / NO_OF_ITERATIONS;
    
    return calculatedData;
}

void sendTimeMessage(int queue_id) {
    printf("Function - Send Time Message with queue id: %d\n", queue_id);
    int count;
    struct timeval send_time;
    msgbuf_t message_buffer;
    data_t *data_ptr = (data_t *)(message_buffer.mtext);

    for (count = 0; count < NO_OF_ITERATIONS; ++count) {
        gettimeofday(&send_time, NULL);
        message_buffer.mtype = MESSAGE_MTYPE_TIME;
        data_ptr->msg_no = count;
        data_ptr->send_time = send_time;
        
        while (msgsnd(queue_id, (struct msgbuf *)&message_buffer, sizeof(data_t), 0) == -1) {
            fprintf(stderr, "Nao foi possivel enviar a mensagem neste momento, tentando novamente.\n");
            exit(1);
        }

        usleep(SENDER_DELAY_TIME);
    }
    return;
}

void sendCalculatedData(int queue_id, data_t *calculatedData) {
    printf("Function - Did Start Send Calculated Data with queue id: %d\n", queue_id);
    msgbuf_t message_buffer;
    data_t *calculatedDataPointer = (data_t *)(message_buffer.mtext);
    message_buffer.mtype = MESSAGE_MTYPE_DATA;
    calculatedDataPointer = calculatedData;
    
    if (msgsnd(queue_id, (struct msgbuf *)&message_buffer, sizeof(data_t), 0) == -1) {
        fprintf(stderr, "Nao foi possivel enviar a mensagem neste momento.\n");
        exit(1);
    }
    printf("Function - Did Finish Send Calculated Data with queue id: %d\n", queue_id);
}

void receiveCalculatedData(int queue_id) {
    printf("Function - Receive Calculated Data with queue_id: %d\n", queue_id);
    msgbuf_t message_buffer;
    data_t *data_ptr = (data_t *)(message_buffer.mtext);
    
    ssize_t receivedBytes = -1;
    
    
    while (receivedBytes <= 0) {
        receivedBytes = msgrcv(queue_id, (struct msgbuf *)&message_buffer, sizeof(data_t), MESSAGE_MTYPE_DATA, 0);
        printf("received bytes = %zd, expectedBytes = %zd", receivedBytes, sizeof(data_t));
        fprintf(stderr, "\nImpossivel receber mensagem dos dados calculados!\n\n");
//        exit(1);
    }

//    printf("received bytes :::::::::::: %zd\n", receivedBytes);
    
    printf("RECEIVER: O tempo medio de transferencia: \t%.12f\n", data_ptr->med);
    printf("RECEIVER: O tempo maximo de transferencia: \t%.12f\n", data_ptr->max);
    printf("RECEIVER: O tempo minimo de transferencia: \t%.12f\n", data_ptr->min);
    printf("RECEIVER: O tempo total de transferencia: \t%.12f\n", data_ptr->total);
}

int createOrGetQueueIdWithKey(key_t key) {
    printf("Criando fila de mensagem com a chave: %d\n", key);
    int queue_id;
    if ((queue_id = msgget(key, IPC_CREAT | 0666)) == -1) {
        fprintf(stderr, "Impossivel criar a fila de mensagens com a chave: %d\n", key);
        exit(1);
    }
    return queue_id;
}

void deleteQueueWithId(int queue_id) {
    printf("Deletando fila de mensagem com o id: %d\n", queue_id);
    if (msgctl(queue_id, IPC_RMID, NULL) == -1) {
        printf("Impossivel remover a fila com id: %d\n", queue_id);
        exit(1);
    }
}
