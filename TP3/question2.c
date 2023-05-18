#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

/* Global variables */
int x = 0;
sem_t sem1, sem2;

/* Thread function */
void a1() {
    printf("We are in a1() X = %d\n", x);
    x = 55;
    sleep(1);
}

void a2() {
    sleep(2);
    printf("We are in a2() X = %d\n", x);
}

void b1() {
    printf("We are in b1() X = %d\n", x);
    x = 8;
    sleep(1);

}

void b2() {
    printf("We are in b2() X = %d\n", x);
    sleep(1);
}

/* Thread functions */

void *p1(void *arg) {
    a1();
    sem_post(&sem1); // signal that a1() is done
    sem_wait(&sem2); // wait for b1() to be done
    a2();
    pthread_exit(NULL);
}

void *p2(void *arg) {
    sem_wait(&sem1); // wait for a1() to be done
    b2();
    b1();
    sem_post(&sem2); // signal that b1() is done
    pthread_exit(NULL);
}

/* Main function */
int main() {
    pthread_t thread1, thread2;
    /* semaphore sync should be initialized to 0 */
    if (sem_init(&sem1, 0, 0) == -1 || sem_init(&sem2, 0, 0) == -1) {
        perror("Could not initialize semaphores");
        exit(2);
    }
    if (pthread_create(&thread1, NULL, p1, NULL) < 0) {
        perror("Error: thread cannot be created");
        exit(1);
    }
    if (pthread_create(&thread2, NULL, p2, NULL) < 0) {
        perror("Error: thread cannot be created");
        exit(1);
    }
    /* wait for created thread to terminate */
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    /* destroy semaphores */
    sem_destroy(&sem1);
    sem_destroy(&sem2);
    exit(0);
}

/**La solution proposée dans le pdf ne marche pas car interblocage.*/