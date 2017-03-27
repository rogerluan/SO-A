//
//  Experimento3.c
//
//  Created on Mar 27th 2017
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
//#include <sys/types.h>          /* for wait() */
#include <sys/wait.h>           /* for wait() */
#include <signal.h>             /* for kill(), sigsuspend(), others */
#include <sys/ipc.h>            /* for all IPC function calls */
#include <sys/shm.h>            /* for shmget(), shmat(), shmctl() */
#include <sys/sem.h>            /* for semget(), semop(), semctl() */
#include <string.h>             /* for strerror() */

/*
 * Constantes Necessarias
 */
#define SEM_KEY 9008
#define SHM_KEY	9009
#define NO_OF_CHILDREN 3
#define SEM_PERMS 0666

/*
 * As seguintes variaveis globais contem informacao importante. A variavel
 * g_sem_id e g_shm_id contem as identificacoes IPC para o semaforo e para
 * o segmento de memoria compartilhada que sao usados pelo programa. A variavel
 * g_shm_addr e um ponteiro inteiro que aponta para o segmento de memoria
 * compartilhada que contera o indice inteiro da matriz de caracteres que contem
 * o alfabeto que sera exibido.
 */
int	g_sem_id;
int	g_shm_id;
int	*g_shm_addr;

/*
 * As seguintes duas estruturas contem a informacao necessaria para controlar
 * semaforos em relacao a "fecharem", se nao permitem acesso, ou
 * "abrirem", se permitirem acesso. As estruturas sao incializadas ao inicio
 * do programa principal e usadas na rotina printChars(). Como elas sao
 * inicializadas no programa principal, antes da criacao dos processos filhos,
 * elas podem ser usadas nesses processos sem a necessidade de nova associacao
 * ou mudancas.
 */
struct sembuf g_sem_op_lock[1];
struct sembuf g_sem_op_unlock[1];

/*
 * O seguinte vetor de caracteres contem o alfabeto que constituira o string
 * que sera exibido.
 */
char g_letters_and_numbers[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz 1234567890";

/*
 * Funcoes
 */
void printChars();

/*
 * Programa Principal
 */
int main(int argc, char *argv[]) {
    /*
     * Para armazenar os ids dos processos filhos, permitindo o posterior
     * uso do comando kill
     */
    int pid[NO_OF_CHILDREN];
    
    /*
     * Construindo a estrutura de controle do semaforo
     */
    g_sem_op_lock[0].sem_num = 0;
    g_sem_op_lock[0].sem_op = -1;
    g_sem_op_lock[0].sem_flg = 0;
    
    /*
     * Pergunta 1: Se usada a estrutura g_sem_op1 ter· qual efeito em um conjunto de sem·foros?
     */
    
    g_sem_op_unlock[0].sem_num = 0;
    g_sem_op_unlock[0].sem_op = 1;
    g_sem_op_unlock[0].sem_flg = 0;
    
    /*
     * Criando o semaforo
     */
    if ((g_sem_id = semget(SEM_KEY, 1, IPC_CREAT | SEM_PERMS)) == -1) {
        fprintf(stderr, "chamada a semget() falhou, impossivel criar o conjunto de semaforos!");
        exit(1);
    }
    
    if (semop(g_sem_id, g_sem_op_lock, sizeof(g_sem_op_lock)) == -1) {
        fprintf(stderr, "chamada semop() falhou, impossivel inicializar o semaforo!");
        exit(1);
    }
    
    /*
     * Pergunta 2: Para que serve esta operacao semop(), se n„o est· na saÌda de uma regi„o crÌtica?
     */
    
    /*
     * Criando o segmento de memoria compartilhada
     */
    if ((g_shm_id = shmget(SHM_KEY, sizeof(int), IPC_CREAT | SEM_PERMS)) == -1) {
        fprintf(stderr, "Impossivel criar o segmento de memoria compartilhada. Erro: %s\n", strerror(errno));
        exit(1);
    }
    if ((g_shm_addr = (int *)shmat(g_shm_id, NULL, 0)) == (int *)-1) {
        fprintf(stderr, "Impossivel associar o segmento de memoria compartilhada! Erro: %s\n", strerror(errno));
        exit(1);
    }
    *g_shm_addr = 0;
    
    /*
     * Pergunta 3: Para que serve essa inicialização da memória compartilhada com zero?
     */
    
    /*
     * Criando os filhos
     */
    int rtn = 1;
    int count;
    for (count = 0; count < NO_OF_CHILDREN; ++count) {
        if (rtn != 0) {
            pid[count] = rtn = fork();
        } else {
            exit(0);
        }
    }
    
    /*
     * Verificando o valor retornado para determinar se o processo e
     * pai ou filho
     */
    if (rtn > 0) {
        // Eu sou um filho
        printf("Filho %i comecou ...\n", count);
        printChars();
    } else {
        usleep(15000);
    
        // Matando os filhos
        int i;
        for (i = 0; i < NO_OF_CHILDREN; ++i) {
            kill(pid[i], SIGKILL);
        }
        
        // Removendo a memoria compartilhada
        if (shmctl(g_shm_id, IPC_RMID, NULL) != 0) {
            fprintf(stderr, "Impossivel remover o segmento de memoria compartilhada!\n");
            exit(1);
        }
        
        // Removendo o semaforo
        if (semctl(g_sem_id, 0, IPC_RMID, 0) != 0) {
            fprintf(stderr, "Impossivel remover o conjunto de semaforos!\n");
            exit(1);
        }
        exit(0);
    }
}

/*
	* Pergunta 4: se os filhos ainda n„o terminaram, semctl e shmctl, com o parametro IPC-RMID, nao
	* permitem mais o acesso ao sem·foro / memÛria compartilhada?
	*/

/*
 * Esta rotina realiza a exibicao de caracteres. Nela e calculado um numero
 * pseudo-randomico entre 1 e 3 para determinar o numero de caracteres a exibir.
 * Se a protecao esta estabelecida, a rotina entao consegue o recurso. Em
 * seguida, PrintChars() acessa o indice com seu valor corrente a partir da
 * memoria compartilhada. A rotina entra em loop, exibindo o numero aleatorio de
 * caracteres. Finalmente, a rotina incrementa o indice, conforme o necessario,
 * e libera o recurso, se for o caso.
 */
void printChars() {
    struct timeval tv;
    int number;
    
    int tmp_index;
    int i;
    
    /*
     * Este tempo permite que todos os filhos sejam iniciados
     */
    usleep(200);
    
    /*
     * Entrando no loop principal
     */
    while(1) {
        
        /*
         * Conseguindo o tempo corrente, os microsegundos desse tempo
         * sao usados como um numero pseudo-randomico. Em seguida,
         * calcula o numero randomico atraves de um algoritmo simples
         */
        if (gettimeofday(&tv, NULL) == -1) {
            fprintf(stderr,"Impossivel conseguir o tempo atual, terminando.\n");
            exit(1);
        }
        number = ((tv.tv_usec / 47) % 3) + 1;
        
        /*
         * Pergunta 5: quais os valores possÌveis de serem atribuidos
         * a number?
         */
        
        /*
         * O #ifdef PROTECT inclui este pedaco de codigo se a macro
         * PROTECT estiver definida. Para sua definicao, retire o comentario
         * que a acompanha. semop() e chamada para fechar o semaforo.
         */
        
#ifdef PROTECT
        if (semop(g_sem_id, g_sem_op1, 1) == -1) {
            fprintf(stderr,"chamada semop() falhou, impossivel fechar o recurso!");
            exit(1);
        }
#endif
        
        /*
         * Lendo o indice do segmento de memoria compartilhada
         */
        tmp_index = *g_shm_addr;
        
        /*
         * Repita o numero especificado de vezes, esteja certo de nao
         * ultrapassar os limites do vetor, o comando if garante isso
         */
        for (i = 0; i < number; ++i) {
            if (!(tmp_index + i > sizeof(g_letters_and_numbers))) {
                fprintf(stderr,"%c", g_letters_and_numbers[tmp_index + i]);
                usleep(1);
            }
        }
        
        /*
         * Atualizando o indice na memoria compartilhada
         */
        
        *g_shm_addr = tmp_index + i;
        
        /*
         * Se o indice e maior que o tamanho do alfabeto, exibe um
         * caractere return para iniciar a linha seguinte e coloca
         * zero no indice
         */
        if (tmp_index + i > sizeof(g_letters_and_numbers)) {
            fprintf(stderr,"\n");
            *g_shm_addr = 0;
        }
        
        /*
         * Liberando o recurso se a macro PROTECT estiver definida
         */
        
#ifdef PROTECT
        if (semop(g_sem_id, g_sem_op1, 1) == -1) {
            fprintf(stderr,"chamada semop() falhou, impossivel liberar o recurso!");
            exit(1);
        }
#endif
    }
}
