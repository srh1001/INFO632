#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

// Cette fonction recherche un pattern dans un fichier.
void chercher_pattern(char* pattern, char* fichier_pointeur) {
    pid_t pid = fork(); // Création d'un nouveau processus fils.

    if (pid == -1) { 
        printf("Erreur lors de la création du processus fils.\n"); 
        exit(1); 
    } else if (pid == 0) { // Si le processus en cours est le processus fils
        printf("Processus fils %d créé pour le fichier %s\n", getpid(), fichier_pointeur); // message pour dire quel processus a été créé pour quel fichier.

        // Commande grep à éxécuter pour chercher le pattern :
        char* cmd = (char*) malloc(strlen(pattern) + strlen(fichier_pointeur) + 7); // allocation de mémoire pour stocker la commande.

        sprintf(cmd, "grep %s %s", pattern, fichier_pointeur); // la commande contient le pattern et le nom de fichier à chercher.

        system(cmd); // exécution de la commande avec la fonction system().

        free(cmd); // libération de la mémoire allouée pour la commande

        exit(0); // quitte le processus fils.
    }
}

// Cette fonction recherche un pattern (en utilisant lauch_grep) dans un ensemble de fichiers.
void chercher_pattern_fichiers(char* pattern, char** fichiers_pointeurs, int nb_fichiers) {
    printf("Fichiers dans lesquels le motif sera cherché :\n");
    
    // Parcours de tous les fichiers dans lequel chercher le pattern:
    for (int i = 0; i < nb_fichiers; i++) {
        printf("%s\n", fichiers_pointeurs[i]); // Affichage du nom du fichier en cours de traitement.
        chercher_pattern(pattern, fichiers_pointeurs[i]); // Recherche du pattern dans le fichier courant.
    }
    
    // Attente de la fin de tous les processus fils :
    for (int i = 0; i < nb_fichiers; i++) {
        wait(NULL);
    }
}


int main(int argc, char *argv[]) {
    
    // Vérification du nombre d'arguments :
    if (argc != 3) {
        printf("Arguments manquants: %s <pattern nb_fichiers>\n", argv[0]);
        return 1;
    }

    char* pattern = argv[1];
    int nb_fichiers = atoi(argv[2]); // conversion de l'argument n°2 en integer, cet argument est le nombre de fichiers à créer.

    // Création de fichiers pour tester les fonctions :
    FILE* fp;
    char* fichiers[nb_fichiers]; // Création d'un tableau de pointeurs vers les noms de fichiers à créer.

    for (int i = 0; i < nb_fichiers; i++) {
        fichiers[i] = malloc(strlen("fichier") + sizeof(int) + strlen(".txt") + 1); // Allocation dynamique de mémoire pour le nom du fichier.
        sprintf(fichiers[i], "fichier%d.txt", i+1); // Construction du nom du fichier.
        printf("Nom du fichier : %s\n", fichiers[i]);

        fp = fopen(fichiers[i], "w"); // Ouverture en écriture du fichier créé.
        fprintf(fp, "C'est le fichier n°%d créé.\nCette ligne contient le pattern AZERTY, à trouver.\n", i+1); // Écriture dans le fichier.
        fclose(fp); // Fermeture du fichier.
    }

    // Appel de la fonction chercher_pattern_fichiers avec les fichiers créés :
    chercher_pattern_fichiers(pattern, fichiers, nb_fichiers);

    // Libération de la mémoire allouée pour les noms de fichiers et suppression des fichiers:
    for (int i = 0; i < nb_fichiers; i++) {
        remove(fichiers[i]);
        free(fichier[i]);
    }

    return 0;
}