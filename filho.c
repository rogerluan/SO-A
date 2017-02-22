#include <sys/time.h>		/* for gettimeofday() */
#include <unistd.h>		/* for gettimeofday() and fork() */
#include <stdio.h>		/* for printf() */
#include <sys/types.h>		/* for wait() */
#include <sys/wait.h>		/* for wait() */

int main(int argc, char *argv[]){
		int count,int no_of_iterations,int micro_per_second,int sleep_time
		count = argv[0];
		
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
			child_no, drift - no_of_iterations*sleep_time/micro_per_second,
			(drift - no_of_iterations *sleep_time/micro_per_second)/no_of_iterations);
		
		/*
		 * Pergunta 5: Qual a relação: entre SLEEP_TIME e o desvio, nenhuma, direta 
		 * ou indiretamente proporcional? 
		 */
		return 0;
}
