#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

void launch_grep(char* pattern, char* file_name) {
    pid_t pid = fork(); // Création d'un nouveau processus fils

    if (pid == -1) { // Si la création du processus fils échoue
        printf("Error: failed to create child process\n"); // affiche un message d'erreur
        exit(1); // quitte le processus avec un code de retour différent de zéro pour signaler une erreur
    } else if (pid == 0) { // Si le processus en cours est le processus fils
        printf("Child process %d created for file %s\n", getpid(), file_name); // affiche un message pour indiquer que le processus fils a été créé

        // allocation de mémoire pour stocker la commande
        char* cmd = (char*) malloc(strlen(pattern) + strlen(file_name) + 7);

        // création de la commande à exécuter avec sprintf()
        // la commande contient le motif et le nom de fichier à chercher
        sprintf(cmd, "grep %s %s", pattern, file_name);

        // exécution de la commande avec la fonction system()
        system(cmd);

        // libération de la mémoire allouée pour la commande
        free(cmd);

        // quitte le processus fils avec un code de retour zéro pour signaler que tout s'est bien passé
        exit(0);
    }
}

// Recherche une chaîne de caractères donnée dans un ensemble de fichiers
void mgrep(char* pattern, char** file_names, int num_files) {
    printf("Fichiers dans lesquels le motif sera cherché :\n");
    
    // Parcourir tous les fichiers à rechercher et lancer la recherche dans chacun d'eux
    for (int i = 0; i < num_files; i++) {
        printf("%s\n", file_names[i]); // Affiche le nom du fichier en cours de traitement
        launch_grep(pattern, file_names[i]); // Lance la recherche dans le fichier courant
    }
    
    // Attendre que tous les processus fils se terminent
    for (int i = 0; i < num_files; i++) {
        wait(NULL); // Attend la fin du processus fils spécifié
    }
}

int main(int argc, char *argv[]) {
    
    // Vérification du nombre d'arguments :
    if (argc != 4) {
        printf("Arguments manquants: %s <pattern max_files max_file_name>\n", argv[0]);
        return 1;
    }

    int max_files = atoi(argv[2]); // conversion de la chaîne de caractères en entier pour déterminer le nombre de fichiers maximum
    int max_file_name = atoi(argv[3]); // conversion de la chaîne de caractères en entier pour déterminer la longueur maximum des noms de fichiers

    // création de quelques fichiers de test
    FILE* fp;
    char file_name[max_files][max_file_name]; // tableau multidimensionnel de noms de fichiers
    for (int i = 0; i < max_files; i++) {
        sprintf(file_name[i], "file%d.txt", i+1); // création d'un nom de fichier en utilisant sprintf avec le numéro incrémenté pour chaque fichier
        fp = fopen(file_name[i], "w"); // ouverture en écriture du fichier avec le nom créé
        fprintf(fp, "This is file %d.\nLine with the value to search.\n", i+1); // écriture de quelques lignes dans le fichier
        fclose(fp); // fermeture du fichier
    }

    // test de la fonction mgrep
    char* pattern = "value"; // chaîne de caractères à rechercher
    char* file_names[max_files]; // tableau de pointeurs vers les noms de fichiers
    for (int i = 0; i < max_files; i++) {
        file_names[i] = file_name[i]; // initialisation des pointeurs avec les noms de fichiers créés précédemment
    }

    mgrep(pattern, file_names, max_files); // appel de la fonction mgrep pour rechercher la chaîne de caractères dans les fichiers

    // suppression des fichiers de test
    for (int i = 0; i < max_files; i++) {
        remove(file_name[i]); // suppression de chaque fichier créé précédemment
    }

    return 0;
}