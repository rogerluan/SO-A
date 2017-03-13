//
//  main.c
//  exp2
//
//  Created by Roger Luan on 13/03/17.
//  Copyright © 2017 Roger Oba. All rights reserved.
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
#define MESSAGE_QUEUE_ID	3102
#define SENDER_DELAY_TIME	10
#define MESSAGE_MTYPE		1
#define NO_OF_CHILDREN 		2 // acrescentando numero de filhos

void Receiver(int queue_id);
void Sender(int queue_id);
/*
 * Pergunta 1: O que eh um protÛtipo? Por qual motivo eh usado?
 */

int main(int argc, char *argv[]) {
    int rtn = 1;				//inicializa rtn
    int count = 0, i = 0;
    
    /*
     * Variaveis relativas a fila, id e key
     */
    int queue_id;
    key_t key = MESSAGE_QUEUE_ID;
    
    /*
     * Cria a fila de mensagens
     */
    if ((queue_id = msgget(key, IPC_CREAT | 0666)) == -1) {
        fprintf(stderr,"Impossivel criar a fila de mensagens!\n");
        exit(1);
    }
    
    /*
     * Pergunta 2: O que significa cada um dos dÌgitos 0666?
     * Pergunta 3: Para que serve o arquivo stderr?
     * Pergunta 4: Caso seja executada a chamada fprintf com o handler stderr, onde aparecer· o seu resultado?
     * Pergunta 5: Onde stderr foi declarado?
     * Pergunta 6: Explicar o que s„o e para que servem stdin e stdout.
     */
    
    for (i = 0; i < NO_OF_CHILDREN; i++) {	//consertando o contador
        if (rtn > 0) {
            rtn = fork();
            count++;
        } else {
            break;
        }
    }
    
    /*
     * Verifica o valor retornado para determinar se o processo eh pai ou filho
     *
     * OBS:  o valor de count no loop anterior indicara cada um dos filhos
     *       count = 1 para o primeiro filho, 2 para o segundo, etc.
     */
    if (rtn == 0 && count == 1) {
        printf("Receptor iniciado ...\n"); 		// fechando aspas
        Receiver(queue_id);
        exit(0);
    } else if (rtn == 0 && count == 2) {		// faltando um = na comparaÁ„o
        printf("Emissor iniciado ...\n");
        Sender(queue_id);
        exit(0);
    } else {
        printf("Pai aguardando os filhos terminarem...\n");
        wait(NULL);
        wait(NULL);
        
        /*
         * Removendo a fila de mensagens
         */
        if (msgctl(queue_id, IPC_RMID, NULL) == 0) {
            printf("Impossivel remover a fila!\n");
            exit(1);
        }
        /*
         * Pergunta 7: O que ocorre com a fila de mensagens, se ela n„o È removida e os
         * processos terminam?
         */
        exit(0);
    }
}

/*
 * O tipo de dados seguinte pode ser usado para declarar uma estrutura que
 * contera os dados que serao transferidos pela fila.  A estrutura vai conter
 * um numero de mensagem (msg_no) e o tempo de envio (send_time).  Para filas
 * de mensagens, o tipo da estrutura pode definir qualquer dado que seja necessario.
 */
typedef struct {
    unsigned int msg_no;
    struct timeval send_time;
} data_t;

/*
 * O conteudo de uma estrutura com o seguinte tipo de dados sera enviado
 * atraves da fila de mensagens. O tipo define dois dados.  O primeiro eh
 * o tipo da mensagem (mtype) que sera como uma identificacao de mensagem.
 * Neste exemplo, o tipo eh sempre o mesmo. O segundo eh um vetor com tamanho
 * igual ao definido pelo tipo declarado anteriormente. Os dados a serem
 * transferidos sao colocados nesse vetor. Na maioria das circunstancias,
 * esta estrutura nao necessita mudar.
 */
typedef struct {
    long mtype;
    char mtext[sizeof(data_t)];
} msgbuf_t;

/*
 * Esta funcao executa o recebimento das mensagens
 */
void Receiver(int queue_id) {
    int count;
    struct timeval receive_time;
    float delta = 0;
    float max = 0;
    float total;
    
    /*
     * Este eh o buffer para receber a mensagem
     */
    msgbuf_t message_buffer;
    
    /*
     * Este e o ponteiro para os dados no buffer.  Note
     * como e setado para apontar para o mtext no buffer
     */
    data_t *data_ptr = (data_t *)(message_buffer.mtext);
    
    // Pergunta 8: Qual ser· o conte˙do de data_ptr?
    
    for (count = 0; count < NO_OF_ITERATIONS; ++count) {
        /*
         * Recebe qualquer mensagem do tipo MESSAGE_MTYPE
         */
        if (msgrcv(queue_id,(struct msgbuf *)&message_buffer,sizeof(data_t),MESSAGE_MTYPE,0) == -1) {
            fprintf(stderr, "Impossivel receber mensagem!\n");
            exit(1);
        }

        gettimeofday(&receive_time,NULL);
        
        delta = receive_time.tv_sec - data_ptr->send_time.tv_sec;
        delta += (receive_time.tv_usec - data_ptr->send_time.tv_usec) / (float)MICRO_PER_SECOND;
        total += delta;
        
        if (delta > max) {
            max = delta;
        }
    }
    printf("O tempo medio de transferencia: \t%.12f\n", total / NO_OF_ITERATIONS);
    printf("O tempo maximo de transferencia: \t%.12f\n", max);
    
    return;
}

/*
 * Esta funcao envia mensagens
 */
void Sender(int queue_id) {
    int count;
    struct timeval send_time;
    
    /*
     * Este e o buffer para as mensagens enviadas
     */
    msgbuf_t message_buffer;
    
    /*
     * Este e o ponteiro para od dados no buffer.  Note
     * como e setado para apontar para mtext no buffer
     */
    data_t *data_ptr = (data_t *)(message_buffer.mtext);

    for (count = 0; count < NO_OF_ITERATIONS; count++) {
        gettimeofday(&send_time, NULL);
        
        /*
         * Apronta os dados
         */
        message_buffer.mtype = MESSAGE_MTYPE;
        data_ptr->msg_no = count;
        data_ptr->send_time = send_time;
        
        /*
         * Envia a mensagem... usa a identificacao da fila, um ponteiro
         * para o buffer, e o tamanho dos dados enviados
         */
        if (msgsnd(queue_id, (struct msgbuf *)&message_buffer,sizeof(data_t),0) == -1) {
            fprintf(stderr, "Impossivel enviar mensagem!\n");
            exit(1);
        }
        
        /*
         * Dorme por um curto espaco de tempo
         */
        usleep(SENDER_DELAY_TIME);
    }
    return;
}
