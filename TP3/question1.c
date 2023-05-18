#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

/* Global variables */
int x = 0;
sem_t sem; //correction provided: previous sync

/* Thread function */
//correction provided: a() and b() placed before the threads *p1 and *p2.


void a() {
    sleep(2);
    printf("X = %d\n", x);
    x=55;
}

void b() {
    printf("X = %d\n", x);
}

void *p1(void *arg) {
    /* send signal to the created thread */
    a();
    sem_post(&sem);
 

    pthread_exit(NULL);

}

void *p2(void *arg) {
    /* wait for signal from main thread */
    sem_wait(&sem);
    b();
    pthread_exit(NULL);
}


void main () {
    pthread_t thread1, thread2;
    /* semaphore sync should be initialized to 0 */
    if (sem_init(&sem, 0, 0) == -1) {
        perror("Could not initialize m ylock semaphore");
        exit(2);
    }
    if (pthread_create(&thread1, NULL, p1, NULL) < 0) {
        perror("Error: thread cannot be created");
        exit(1);
    }
    if (pthread_create(&thread2, NULL, p2, NULL) < 0) { // correction provided: p2
        perror("Error: thread cannot be created");
        exit(1);
    }
    /* wait for created thread to terminate */
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    /* destroy semaphore sem */
    sem_destroy(&sem);
    exit(0);
}
