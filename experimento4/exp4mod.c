//Includes Necessarios

#include <pthread.h>			/* para poder manipular threads */
#include <stdio.h>			/* para printf() */
#include <stdlib.h>
#include <unistd.h>     // usleep();
#include <stdbool.h>

#define NUM_OF_PHILOSOPHERS 5
#define NUM_OF_ITERATIONS 365

#define LEFT (n + NUM_OF_PHILOSOPHERS - 1) % NUM_OF_PHILOSOPHERS // Calcule the left position in a circular array
#define RIGHT (n + 1) % NUM_OF_PHILOSOPHERS // Calcule the right position in a circular array

typedef enum {
    statusThinking,
    statusWaiting,
    statusEating
} status;

status states[NUM_OF_PHILOSOPHERS];
pthread_t philosophers[NUM_OF_PHILOSOPHERS];
pthread_mutex_t chopsticks[NUM_OF_PHILOSOPHERS];
pthread_mutex_t mutex;

void printStatus() {
    fprintf(stdout, "\nPrinting the status of all 5 philosophers:\n");
    fflush(stdout);
    int i;
    for (i = 0; i < NUM_OF_PHILOSOPHERS; ++i) {
        switch (states[i]) {
            case statusThinking:
                fprintf(stdout, "\nPhilosopher %d is thinking.", i);
                break;
            case statusWaiting:
                fprintf(stdout, "\nPhilosopher %d is waiting to eat.", i);
                break;
            case statusEating:
                fprintf(stdout, "\nPhilosopher %d is eating.", i);
                break;
        }
        fflush(stdout);
    }
}

void think(int n) {
    states[n] = statusThinking;
    usleep(25);
}

void eatIfPossible(int n) {
    printStatus();
    if (states[n] == statusWaiting && states[LEFT] != statusEating && states[RIGHT] != statusEating) {
        states[n] = statusEating;
        fprintf(stdout, "\nPhilosopher %d has started eating now.\n", n);
        fflush(stdout);
        pthread_mutex_unlock(&chopsticks[n]);
    }
}

void takeChopsticks(int n) {
    pthread_mutex_lock(&mutex);
    states[n] = statusWaiting;
    eatIfPossible(n);
    pthread_mutex_unlock(&mutex);
    pthread_mutex_lock(&chopsticks[n]);
}

void putChopsticks(int n) {
    pthread_mutex_lock(&mutex);
    states[n] = statusThinking; // We shouldn't call think(); here
    eatIfPossible(LEFT);
    eatIfPossible(RIGHT);
    pthread_mutex_unlock(&mutex);
}

void *thinkAndEat(int n) {
    int i;
    for (i = 0; i < NUM_OF_ITERATIONS; ++i) {
        think(n);
        takeChopsticks(n);
        putChopsticks(n);
    }
    fprintf(stdout, "Philosopher %d finished eating %d times\n", n, NUM_OF_ITERATIONS);
    fflush(stdout);   
    return(NULL);
}

void cleanEatingStates() {
    int i;
    for (i = 0; i < NUM_OF_PHILOSOPHERS; ++i) {
        states[i] = statusThinking;
    }
}

int main(int argc, char *argv[]) {
    cleanEatingStates();
    
    int i;
    // Initialize 5 mutex
    for (i = 0; i < NUM_OF_PHILOSOPHERS; i++) {
        pthread_mutex_init(&chopsticks[i], NULL);
    }
    pthread_mutex_init(&mutex, NULL);
    
    // Create 5 threads and execute the function on each of them
    for (i = 0; i < NUM_OF_PHILOSOPHERS; ++i) {
        pthread_create(&philosophers[i], NULL, (void *)thinkAndEat, (void *)i);
    }
    
    // Waits for the termination of all 5 threads
    for (i = 0; i < NUM_OF_PHILOSOPHERS; ++i) {
        pthread_join(philosophers[i],NULL);
    }
    
    // Destroy all 5 mutex
    for (i = 0; i < NUM_OF_PHILOSOPHERS; ++i) {
        pthread_mutex_destroy(&chopsticks[i]);
    }
    pthread_mutex_destroy(&mutex);
    
    return 0;
}
