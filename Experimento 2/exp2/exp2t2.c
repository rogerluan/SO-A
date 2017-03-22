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

/*
 * Includes Necessarios
 */
#include <sys/time.h>   // gettimeofday()
#include <stdio.h>		// printf()
#include <stdlib.h>     // exit()
#include <unistd.h>		// fork()
#include <sys/msg.h>		/* for msgget(), msgctl() */
#include <sys/wait.h>		/* for wait() */

#define NO_OF_ITERATIONS	500
#define MICRO_PER_SECOND	1000000
#define MESSAGE_QUEUE_KEY1	3102
#define MESSAGE_QUEUE_KEY2	6204
#define SENDER_DELAY_TIME	10
#define MESSAGE_MTYPE_TIME  1
#define MESSAGE_MTYPE_DATA  2
#define NO_OF_CHILDREN 		2

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

//********************** Functions Declarations **********************//

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

//********************** Implementation **********************//

int main(int argc, char *argv[]) {
    int rtn = 1;
    int count = 0, i = 0;
    
    for (i = 0; i < NO_OF_CHILDREN; i++) {
        if (rtn > 0) {
            rtn = fork();
            count++;
        } else {
            break;
        }
    }
    
    if (rtn == 0 && count == 1) {
        int first_queue_id = createOrGetQueueIdWithKey(MESSAGE_QUEUE_KEY1);
        printf("Receptor iniciado...\n");
        data_t *calculatedData = receiveTimeMessage(first_queue_id);
        deleteQueueWithId(first_queue_id);
        
        pid_t pid = fork();
        if (pid == -1) {
            fprintf(stderr, "Ocorreu um erro ao criar o terceiro filho.\n");
        } else if (pid == 0) {
            int second_queue_id = createOrGetQueueIdWithKey(MESSAGE_QUEUE_KEY2);
            sendCalculatedData(second_queue_id, calculatedData);
            exit(0);
        } else if (pid > 0) {
            printf("Terceiro filho iniciado...\n");
            int second_queue_id = createOrGetQueueIdWithKey(MESSAGE_QUEUE_KEY2);
            receiveCalculatedData(second_queue_id);
            wait(NULL);
            deleteQueueWithId(second_queue_id);
            exit(0);
        }
    } else if (rtn == 0 && count == 2) {
        printf("Emissor iniciado...\n");
        int first_queue_id = createOrGetQueueIdWithKey(MESSAGE_QUEUE_KEY1);
        sendTimeMessage(first_queue_id);
        exit(0);
    } else {
        printf("Pai aguardando os filhos terminarem...\n");
        wait(NULL);
        wait(NULL);
        printf("Pai terminando...\n");
        exit(0);
    }
}

data_t *receiveTimeMessage(int queue_id) {
    int count;
    struct timeval receive_time;
    float delta = 0;
    float max = 0;
    float min = 1000;
    float total = 0;
    
    msgbuf_t message_buffer;
    data_t *data_ptr = (data_t *)(message_buffer.mtext);
    
    for (count = 0; count < NO_OF_ITERATIONS; ++count) {
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
    
    data_t *calculatedData = malloc(sizeof(data_t));
    calculatedData->min = min;
    calculatedData->max = max;
    calculatedData->total = total;
    calculatedData->med = total / NO_OF_ITERATIONS;
    
    return calculatedData;
}

void sendTimeMessage(int queue_id) {
    printf("Enviando as mensagens de medicao de tempo...\n");
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
    printf("Todas as mensagens foram enviadas com sucesso!\n");
}

void sendCalculatedData(int queue_id, data_t *calculatedData) {
    printf("Enviando os dados calculados...\n");
    msgbuf_t message_buffer;
    data_t *calculatedDataPointer = (data_t *)(message_buffer.mtext);
    message_buffer.mtype = MESSAGE_MTYPE_DATA;
    calculatedDataPointer->min = calculatedData->min;
    calculatedDataPointer->max = calculatedData->max;
    calculatedDataPointer->med = calculatedData->med;
    calculatedDataPointer->total = calculatedData->total;
    
    if (msgsnd(queue_id, (struct msgbuf *)&message_buffer, sizeof(data_t), 0) == -1) {
        fprintf(stderr, "Nao foi possivel enviar a mensagem neste momento.\n");
        exit(1);
    } else {
        printf("Os dados calculados foram enviados com sucesso!\n");
    }
}

void receiveCalculatedData(int queue_id) {
    printf("Recebendo os dados calculados...\n");
    msgbuf_t message_buffer;
    data_t *data_ptr = (data_t *)(message_buffer.mtext);
    
    if (msgrcv(queue_id, (struct msgbuf *)&message_buffer, sizeof(data_t), MESSAGE_MTYPE_DATA, 0) == -1) {
        fprintf(stderr, "\nImpossivel receber mensagem dos dados calculados!\n");
        exit(1);
    } else {
        printf("Os dados calculados foram recebidos com sucesso!\n");
    }
    
    printf("Receptor: O tempo minimo de transferencia foi: \t%.12f\n", data_ptr->min);
    printf("Receptor: O tempo maximo de transferencia foi: \t%.12f\n", data_ptr->max);
    printf("Receptor: O tempo total de transferencia foi: \t%.12f\n", data_ptr->total);
    printf("Receptor: O tempo medio de transferencia foi: \t%.12f\n", data_ptr->med);
}

int createOrGetQueueIdWithKey(key_t key) {
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
        fprintf(stderr, "Impossivel remover a fila com id: %d\n", queue_id);
        exit(1);
    }
}
