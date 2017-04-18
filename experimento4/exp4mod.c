//Includes Necessarios

#include <pthread.h>			/* para poder manipular threads */
#include <stdio.h>			/* para printf() */
#include <stdlib.h>
#include <unistd.h>     // usleep();
#include <stdbool.h>

#define NUM_OF_PHILOSOPHERS 5
#define NUM_OF_ITERATIONS 365

pthread_t philosopher[NUM_OF_PHILOSOPHERS];
pthread_mutex_t chopstick[NUM_OF_PHILOSOPHERS];

void think(int n) {
    //fprintf(stdout, "Philosopher %d is thinking\n", n);
    //fflush(stdout);
    usleep(25);
}

bool canEat(int n) {
    return true;
}

void printStatus() {
    fprintf(stdout, "Printing the status of all 5 philosophers");
    fflush(stdout);
}

void *func(int n) {
    int i;
    for (i = 0; i < NUM_OF_ITERATIONS; ++i) {
        think(n);
        // When philosopher 0 is eating, he takes forks 0 and 1
        // When philosopher 1 is eating, he takes forks 1 and 2
        // When philosopher 2 is eating, he takes forks 2 and 3
        // When philosopher 3 is eating, he takes forks 3 and 4
        // When philosopher 4 is eating, he takes forks 4 and 0
        pthread_mutex_lock(&chopstick[n]);
        pthread_mutex_lock(&chopstick[(n+1)%NUM_OF_PHILOSOPHERS]);
        fprintf(stdout, "Philosopher %d is eating\n",n);
        fflush(stdout);
        sleep(3);
        pthread_mutex_unlock(&chopstick[n]);
        pthread_mutex_unlock(&chopstick[(n+1)%5]);
    }
    
    fprintf(stdout, "Philosopher %d finished eating %d times\n", n, NUM_OF_ITERATIONS);
    fflush(stdout);
        
    return(NULL);
}

int main() {
    int i;
    // Initialize 5 mutex
    for (i = 0; i < NUM_OF_PHILOSOPHERS; i++) {
        pthread_mutex_init(&chopstick[i], NULL);
    }
    
    // Create 5 threads and execute the function on each of them
    for (i = 0; i < NUM_OF_PHILOSOPHERS; ++i) {
        pthread_create(&philosopher[i], NULL, (void *)func, (void *)i);
    }
    
    // Waits for the termination of all 5 threads
    for (i = 0; i < NUM_OF_PHILOSOPHERS; ++i) {
        pthread_join(philosopher[i],NULL);
    }
    
    // Destroy all 5 mutex
    for (i = 0; i < NUM_OF_PHILOSOPHERS; ++i) {
        pthread_mutex_destroy(&chopstick[i]);
    }
    
    return 0;
}
