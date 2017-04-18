//
//  exp4
//
//  Created on Apr 17th 2017
//
//  ALEX VENTURINI  15294739
//  BRUNO PEDROSO   12662136
//  LUAN BONOMI     15108780
//  PEDRO CATALINI  15248354
//  ROGER OBA       12048534
//

/***********************************************************************************
 *
 * Este programa não faz parte do curso sobre tempo real do Laboratorio Embry-Riddle
 * embora tenha sido inspirado pelos demais experimentos lah existentes.
 *
 * Experimento # 4 na disciplina de Sistemas Operacionais da PUC-Campinas
 * Originalmente programado por Florian Weizenegger
 *							Data: 25/08/2003
 *
 *       Proposito: O proposito deste programa e o de permitir ao aluno perceber
 *       o que vem a ser um thread, de maneira tal que consiga distingui-lo de
 *       um processo. Além disso, são usados os principais comandos para criação
 *	e manipulação de threads.
 *	O problema dos produtores e consumidores sobre um buffer circular é
 *	usado como assunto, permitindo que o aluno experimente duas implementações
 *	diferentes para sua solução. Desta maneira, além dos threads propriamente
 *	ditos, tambem locks e semaforos sao usados para garantir sincronizacao
 *	de threads.
 *
 *************************************************************************************/

/*
 * Includes Necessarios
 */

#include <pthread.h>			/* para poder manipular threads */
#include <stdio.h>			/* para printf() */
#include <stdlib.h>

/*
 * Constantes Necessarias
 */
#define NUM_THREADS     10
#define SIZEOFBUFFER    50
#define NO_OF_ITERATIONS 100

/*
 * O tipo pthread_t permite a declaração de uma variável que recebe
 * um id quando o thread é criado. Posteriormente, esse id pode ser
 * usado em comandos de controle para threads.
 * Seguem dois vetores para ids, para um numero de threads igual a
 * constante NUM_THREADS
 */
pthread_t consumers[NUM_THREADS];
pthread_t producers[NUM_THREADS];

/*
 * Variaveis Necessarias
 */
int buffer[SIZEOFBUFFER];		/* Este e um buffer circular	*/
int *start;				/* apontara para a primeira posicao do buffer */
int *readPointer;				/* eh o apontador para o proximo item do buffer a ser consumido */
int *writePointer;				/* eh o apontador para o proximo item do buffer a ser produzido */
int cont_p = 0;             		/* eh um contador para controlar o numero de itens produzidos */
int cont_c = 0;         		/* eh um contador para controlar o numero de itens consumidos */


/*
 * Rotina para produzir um item toAdd no buffer
 */
int myadd(int toAdd) {
    if ((readPointer != (writePointer+1)) && (readPointer + SIZEOFBUFFER - 1 != writePointer)) {
        writePointer++;
        //verificacao se writePointer chegou a ultima posicao do buffer
        if (writePointer == (start + SIZEOFBUFFER)) {
            writePointer = start;
        }
        *writePointer = toAdd;
        return 1;
    } else {
        return 0;
    }
}

/*
 * Rotina para consumir um item do buffer e coloca-lo em retValue
 */
int myremove() {
    //verificacao se o buffer nao esta vazio
    if (writePointer != readPointer ) {
        int retValue = *readPointer;
        readPointer++;
        //verificacao se readPointer chegou a ultima posicao do buffer
        if (readPointer == (start + SIZEOFBUFFER)) {
            readPointer = start;				/* realiza a circularidade no buffer */
        }
        return retValue;
    } else {
        return 0;
    }
}

/*
 * A rotina produce e responsavel por chamar myadd para que seja
 * colocado o valor 10 em uma posicao do buffer NO_OF_ITERATIONS vezes
 */
void *produce(void *threadid) {
    int sum = 0;
    int ret = 0;
    
    printf("Produtor #%d iniciou...\n", (int)threadid);
    while (cont_p < NO_OF_ITERATIONS) {
        ret = myadd(10);
        if (ret) {
            /*
             * Pergunta 1: porque ret não está sendo comparado a algum valor?
             * Pergunta 2: porque nao ha necessidade de um cast?
             */
            cont_p++;
            sum += 10;
        }
    }
    printf("Soma produzida pelo Produtor #%d : %d\n", (int)threadid, sum);
    pthread_exit(NULL);
    return 0;
}

/*
 * A rotina consume e responsavel por chamar myremove para que seja
 * retorando um dos valores existentes no buffer NO_OF_ITERATIONS vezes
 */
void *consume(void *threadid) {
    int sum = 0;
    int ret = 0;
    int cont_c = 0;
    
    printf("Consumidor #%d iniciou...\n", (int)threadid);
    while (cont_c < NO_OF_ITERATIONS) {
        ret = myremove();
        if (ret > 0) {
            sum += 10;
        }
        cont_c++;
    }
    printf("Soma do que foi consumido pelo Consumidor #%d : %d\n", (int)threadid, sum);
    pthread_exit(NULL);
    return 0;
}

/*
 * Rotina Principal (que tambem e a thread principal, quando executada)
 */
int main(int argc, char *argv[]) {
    int tp[NUM_THREADS], tc[NUM_THREADS];
    int i;
    
    start = &buffer[0];
    writePointer = start + SIZEOFBUFFER - 1;
    readPointer = start;
    
    for (i = 0; i < NUM_THREADS; ++i) {
        /*
         * Pergunta 3: para que serve cada um dos argumentos usados com pthread_create?
         */
        
        if ((tc[i] = pthread_create(&consumers[i], NULL, consume, (void *)i + 1))) {
            printf("ERRO: impossivel criar um thread consumidor\n");
            exit(-1);
        }
        
        if ((tp[i] = pthread_create(&producers[i], NULL, produce, (void *)i + 1))) {
            printf("ERRO: impossivel criar um thread rodutor\n");
            exit(-1);
        }
    }
    
    for (i = 0; i < NUM_THREADS; ++i) {
        pthread_join(tc[i], NULL);
        pthread_join(tp[i], NULL);
    }
    printf("Esperando as threads terminarem main()\n"); // avisa q ainda tem threads para serem terminadas.
    pthread_exit(NULL);
    
    /* 
     * Pergunta 4: O que ocorre com as threads criadas, se ainda
     * estiverem sendo executadas e a thread que as criou termina
     * através de um pthread_exit()?
     */
    
    /*
     * Pergunta 5: Idem à questão anterior, se o termino se da atraves
     * de um exit()?
     */
}
