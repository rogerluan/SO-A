/*******************************************************************************
 * Este programa esta baseado no segundo experimento do curso sobre tempo real
 * do Laboratorio Embry-Riddle
 *
 * Seguem os comentarios originais:
 *
 * Experiment #2: Multi-Tasking, Measuring Drift
 *
 *    Programmer: Eric Sorton
 *          Date: 1/27/97
 *           For: MSE599, Special Topics Class
 *
 *       Purpose: When a basic sleep call is used to determine the precise time
 *                when an action will occur the problem of drift occurs.
 *                The measurement of time is imprecise.  Similarly, the time in
 *                which the sleep call returns is imprecise.  Over time, this
 *                will cause the ocurrence of time to drift.  Just as if a clock
 *                loses 1 second every day, over one day, it is significant, but
 *                over a year, it loses 365 seconds, which is over 6 minutes.
 *                This is an example of drift.
 *
 *       Proposito: Quando uma chamada b�sica sleep e usada para determinar o
 *                instante exato em que alguma acao vai ocorrer, ocorre o problema
 *                do desvio. A medicao de tempo e imprecisa. Similarmente, o tempo
 *                que demora o retorno da chamada sleep tambem e impreciso. Ao
 *                longo do tempo, isto ocasionara um desvio de tempo. Algo como se
 *                um relogio perdesse um segundo a cada dia. Ao longo de um dia,
 *                essa diferenca e insignificante, mas, ao longo de um ano, sao
 *                perdidos 365 segundos, o que e superior a 6 minutos. Este e um
 *                exemplo de desvio.
 *
 *******************************************************************************/

/*
 * Includes Necessarios, verifique se as bibliotecas no diretorio sys/ estao
 * lah. Caso nao estejam, verifique onde estao e altere o include
 */

/**

Integrantes do Grupo - Sistemas Operacionais A

ALEX VENTURINI  15294739
BRUNO PEDROSO   12662136
LUAN BONOMI     15108780
PEDRO CATALINI  15248354
ROGER OBA       12048534

*/


#include <sys/time.h>		/* for gettimeofday() */
#include <unistd.h>		/* for gettimeofday() and fork() */
#include <stdio.h>		/* for printf() */
#include <sys/types.h>		/* for wait() */
#include <sys/wait.h>		/* for wait() */
#include <stdlib.h>
#include <signal.h>
/*
 * Pergunta 1: o que o compilador gcc faz com o arquivo .h, cujo nome aparece ap�s o include?
 */

/*
 * Pergunta 2: apresentar (parcialmente) e explicar o que h� em <stdio.h>
 */

/*
 * Pergunta 3: qual eh a funcao da diretiva include (linha que come�a com #), com rela��o ao compilador?
 */

/*
 * NO_OF_ITERATIONS e o numero de vezes que vai se repetir o loop existente
 * em cada futuro processo filho.
 */

#define NO_OF_ITERATIONS	1000

/*
 * NO_OF_CHILDREN eh o numero de filhos a serem criados, cada qual responsavel
 * pela medida do desvio.
 */

#define NO_OF_CHILDREN	5

/*
 * SLEEP_TIME corresponde a quantidade de tempo para ficar bloqueado.
 */

#define SLEEP_TIME 200

/*
 * MICRO_PER_SECOND define o numero de microsegundos em um segundo
 */

#define MICRO_PER_SECOND	1000000

/*
 * Programa Principal. Contem tanto o codigo para o processo pai como
 * o codigo dos futuros filhos
 */

int main( int argc, char *argv[] )
{
    /*
     * Pergunta 4: o que s�o e para que servem argc e argv? N�o esqueca de
     * considerar o * antes de argv.
     */
    /*
     * start_time e stop_time conterao o valor de tempo antes e depois
     * que as trocas de contexto comecem
     */
    
    //struct timeval start_time;
    //struct timeval stop_time;
    
    /*
     * Outras variaveis importantes
     */
    
    // float drift;
    int count;
    // int child_no;
    
    /*
     * Criacao dos processos filhos
     */
    
    pid_t rtn = 1;
    int sleepTime = 200;
    for( count = 0; count < NO_OF_CHILDREN; count++ ) {
        if( rtn > 0 ) {
            sleepTime+=SLEEP_TIME;
            rtn = fork();
        } else {
            break;
        }
    }
    
    
    /*
     * Verifica-se rtn para determinar se o processo eh pai ou filho
     */
    
    if( rtn == 0 ) {
        char countChar[256];
        char iterationsChar[256];
        char microChar[256];
        char sleepChar[256];
        
        sprintf(countChar, "%d", count);
        sprintf(iterationsChar, "%d", NO_OF_ITERATIONS);
        sprintf(microChar, "%d", MICRO_PER_SECOND);
        sprintf(sleepChar, "%d", sleepTime);
        
        char *my_args[6];
        my_args[0] = "filho";
        my_args[1] = countChar;
        my_args[2] = iterationsChar;
        my_args[3] = microChar;
        my_args[4] = sleepChar;
        my_args[5] = NULL;
        
        execv ("filho", my_args);
    } else {
        /*
         * Sou pai, aguardo o termino dos filhos
         */
        for( count = 0; count < NO_OF_CHILDREN; count++ ) {
            wait(NULL);
        }
        kill(rtn, SIGTERM);
        exit(0);
    }
}

