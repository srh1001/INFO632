#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int main(int argc, char* argv[]) {

    // Vérification du nombre d'arguments :
    if (argc != 2) {
        printf("Argument manquant: %s <n>\n", argv[0]);
        return 1;
    }
    // Conversion du premier argument en entier :
    int n = atoi(argv[1]);
    int i;

    pid_t pid;

    // Affichage des informations du processus parent :
    printf("Process de départ : pid = %d, ppid = %d\n", getpid(), getppid());

    for (i = 1; i <= n; i++) {

        pid = fork();

        if (pid == -1) { // Si le fork a échoué :
           
            printf("Erreur lors de la création du processus fils.\n");
            exit(1);

        } else if (pid == 0) { // Si c'est le processus fils qui s'éxécute :
            
            printf("Processus fils n°%d : pid = %d | ppid = %d \n", i, getpid(), getppid());

        } else { // Si c'est le processus parent qui s'éxécute :

            sleep(10); // Attente avant de terminer, pour avoir le temps de faire pstree -p dans la console.
            wait(NULL); // Attente de la fin du processus fils.
            exit(0);  // Fin du processus parent.
        }
    }

    return 0;
}