#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <ctype.h>

#define SIZE 2
#define NB_MESSAGES 3
#define NB_PRODUCERS 2
#define NB_CONSUMERS 2

char tampon[SIZE][50];
int produced_messages = 0;
int consumed_messages = 0;

// Structure pour représenter la file d'attente (queue)
struct Queue {
    int front, rear, size;
    unsigned capacity;
    char** array;
};

struct Queue* queue; // File d'attente partagée entre les producteurs et les consommateurs
sem_t sprod, scons; // Sémaphores pour la synchronisation

// Fonction pour créer une file d'attente de capacité donnée.
struct Queue* createQueue(unsigned capacity) {
    struct Queue* queue = (struct Queue*) malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity - 1;
    queue->array = (char**) malloc(queue->capacity * sizeof(char*));
    return queue;
}

// Vérifie si la file d'attente est pleine.
int isFull(struct Queue* queue) {
    return (queue->size == queue->capacity);
}

// Vérifie si la file d'attente est vide.
int isEmpty(struct Queue* queue) {
    return (queue->size == 0);
}

// Ajoute un message à la file d'attente.
void enqueue(struct Queue* queue, char* message) {
    if (isFull(queue))
        return;
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->array[queue->rear] = message;
    queue->size++;
    printf("Message enqueued: %s\n", message);
}

// Retire un message de la file d'attente.
char* dequeue(struct Queue* queue) {
    if (isEmpty(queue))
        return NULL;
    char* message = queue->array[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;
    return message;
}

void produire(int n) {
    // fonction qui produit un message
    sprintf(tampon[n % SIZE], "message : %d", n);
    printf("Production n°%d : %s\n", n, tampon[n % SIZE]);
}

void consommer(char* message) {
    // fonction qui consomme un message
    for (int i = 0; message[i] != '\0'; i++) {
        message[i] = toupper(message[i]);
    }
    printf("Consommation n°%d : %s\n", consumed_messages, message);
}

// Fonction de production exécutée par les threads producteurs.
void* deposer(void* arg) {
    while (1) {
        if (produced_messages >= NB_MESSAGES)
            pthread_exit(NULL);
        sem_wait(&sprod);
        produire(produced_messages);
        enqueue(queue, tampon[produced_messages % SIZE]);
        produced_messages++;
        sem_post(&scons);
    }
}

// Fonction de consommation exécutée par les threads consommateurs.
void* retirer(void* arg) {
    char* message;
    while (1) {
        if (consumed_messages >= NB_MESSAGES)
            pthread_exit(NULL);
        sem_wait(&scons);
        message = dequeue(queue);
        consommer(message);
        consumed_messages++;
        sem_post(&sprod);
        sleep(2);
    }
}

int main() {
    queue = createQueue(SIZE);
    pthread_t producer_threads[NB_PRODUCERS];
    pthread_t consumer_threads[NB_CONSUMERS];

    // Initialisation des sémaphores
    sem_init(&sprod, 0, SIZE);
    sem_init(&scons, 0, 0);

    // Création des threads producteurs
    for (int i = 0; i < NB_PRODUCERS; i++) {
        if (pthread_create(&producer_threads[i], NULL, deposer, NULL) != 0) {
            perror("Erreur lors de la création du thread producteur");
            exit(1);
        }
    }

    // Création des threads consommateurs
    for (int i = 0; i < NB_CONSUMERS; i++) {
        if (pthread_create(&consumer_threads[i], NULL, retirer, NULL) != 0) {
            perror("Erreur lors de la création du thread consommateur");
            exit(1);
        }
    }

    // Attente de la fin des threads producteurs
    for (int i = 0; i < NB_PRODUCERS; i++) {
        if (pthread_join(producer_threads[i], NULL) != 0) {
            perror("Erreur lors de l'attente du thread producteur");
            exit(1);
        }
    }

    // Attente de la fin des threads consommateurs
    for (int i = 0; i < NB_CONSUMERS; i++) {
        if (pthread_join(consumer_threads[i], NULL) != 0) {
            perror("Erreur lors de l'attente du thread consommateur");
            exit(1);
        }
    }

    // Destruction des sémaphores
    sem_destroy(&sprod);
    sem_destroy(&scons);

    return 0;
}