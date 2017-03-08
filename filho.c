
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

int main(int argc, char *argv[]){
    int count;
    int no_of_iterations;
    int micro_per_second;
    int sleep_time;
    
    if (argc >= 4) {
        sleep_time = atoi(argv[4]);
    }
    if (argc >= 3) {
        micro_per_second = atoi(argv[3]);
    }
    if (argc >= 2) {
        no_of_iterations = atoi(argv[2]);
    }
    if (argc >= 1) {
        count = atoi(argv[1]);
    }
    
    printf("\n\n\nChild Number = %d\nIteractions = %d\nMicroPerSec = %d\nSleep Time = %d\n", count, no_of_iterations, micro_per_second, sleep_time);
    
    float drift;
    int child_no;
    struct timeval start_time;
    struct timeval stop_time;
    
    child_no = count;
    
    /*
     * Primeiro, obtenho o tempo inicial.
     */
    gettimeofday( &start_time, NULL );
    
    
    /*
     * Este loop ocasiona a minha dormencia, de acordo com
     * SLEEP_TIME, tantas vezes quanto NO_OF_ITERATIONS
     */
    for( count = 0; count < no_of_iterations; count++ ) {
        usleep(sleep_time);
    }
    
    /*
     * Paraobter o tempo final
     */
    gettimeofday( &stop_time, NULL );
    
    /*
     * Calcula-se o desvio
     */
    drift = (float)(stop_time.tv_sec  - start_time.tv_sec);
    drift += (stop_time.tv_usec - start_time.tv_usec)/(float)micro_per_second;
    
    /*
     * Exibe os resultados
     */
    printf("Filho #%d -- desvio total: %.5f -- desvio medio: %.5f\n",
           child_no, drift - no_of_iterations*sleep_time/(float)micro_per_second,
           (drift - no_of_iterations *sleep_time/(float)micro_per_second)/no_of_iterations);
    
    /*
     * Pergunta 5: Qual a relação: entre SLEEP_TIME e o desvio, nenhuma, direta 
     * ou indiretamente proporcional? 
     */
    wait(NULL);
//    return 0;
}
