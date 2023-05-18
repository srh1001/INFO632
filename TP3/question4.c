#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <semaphore.h>

#define SIZE 2 // nombre de cases du tampon.
#define NB_MESSAGES 4 // nombre total de messages à produire.

sem_t sprod, scons; // déclaration des sémaphores.
char tampon[SIZE][50]; // tampon pour stocker les messages.
int produced_messages = 0; // nombre de messages produits.
int consumed_messages = 0; // nombre de messages consommés.

// Fonction qui produit le n ième message.
void produire(int n) { 
    sprintf(tampon[n % SIZE], "message : %d", n); // Stocke le message dans la case n du tampon.
    printf("Production n°%d : %s\n", n, tampon[n % SIZE]); // Affichage du message produit.
}

// Fonction qui consomme un message.
void consommer(char* message) {
    for (int i = 0; message[i] != '\0'; i++) {
        message[i] = toupper(message[i]); // Convertit les caractères du message en majuscules.
    }
    printf("Consommation n°%d : %s\n", consumed_messages, message); // Affichage du message consommé.
}

// Fonction qui dépose un message dans le tampon en synchronisant avec les retraits.
void* deposer(void* arg) {
    while (produced_messages < NB_MESSAGES) {
        sem_wait(&sprod); // Attente de la libération d'une case dans le tampon (i.e. consommation d'un message).
        produire(produced_messages); // Production du message et dépôt de celui-ci dans le tampon.
        produced_messages++; // Incrémente le nombre de messages produits.
        sem_post(&scons); // Signal pour indiquer qu'un message a été produit.
    }
    pthread_exit(NULL);
}

// Fonction qui consomme un message dans le tampon en synchronisant avec les dépôts.
void* retirer(void* arg) {
    while (consumed_messages < NB_MESSAGES) {
        sem_wait(&scons); // Attente d'un signal de production d'un message.
        char* message = tampon[consumed_messages % SIZE]; // Récupèration du message dans le tampon.
        consommer(message); // Consommation du message.
        consumed_messages++; // Incrémente le nombre de messages consommés.
        sem_post(&sprod); // Signal pour indiquer qu'un message a été consommé.
    }
    pthread_exit(NULL); // Quitte le thread si le nombre maximal de messages consommés est atteint.

}

int main() {
    sem_init(&sprod, 0, SIZE); // Initialisation du sémaphore de production avec la taille du tampon
    sem_init(&scons, 0, 0); // Initialisation du sémaphore de consommation avec 0 (aucun message disponible)

    pthread_t producer_thread, consumer_thread;

    // Création du thread producteur
    if (pthread_create(&producer_thread, NULL, deposer, NULL) != 0) {
        perror("Erreur lors de la création du thread producteur");
        exit(1);
    }

    // Création du thread consommateur
    if (pthread_create(&consumer_thread, NULL, retirer, NULL) != 0) {
        perror("Erreur lors de la création du thread consommateur");
        exit(1);
    }

    // Attente de la fin du thread producteur
    if (pthread_join(producer_thread, NULL) != 0) {
        perror("Erreur lors de l'attente du thread producteur");
        exit(1);
    }

    // Attente de la fin du thread consommateur
    if (pthread_join(consumer_thread, NULL) != 0) {
        perror("Erreur lors de l'attente du thread consommateur");
        exit(1);
    }

    sem_destroy(&sprod); 
    sem_destroy(&scons); 

    return 0;
}