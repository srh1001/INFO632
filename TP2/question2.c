#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

// Cette fonction crée un arbre binaire de processus de façon récursive.
// n: le nombre de niveaux de l'arbre.
// pid: le pid du processus parent.
void create_two_children(int n, int pid) {
    int i;
    // boucle pour créer 2 processus enfants :
    for (i = 0; i < 2; i++) {

        pid_t child_pid = fork();

        if (child_pid == -1) {

            printf("Erreur lors de la création du processus fils.\n");
            exit(1);

        } else if (child_pid == 0) {

            printf("Process fils %d créé avec le parend %d.\n", getpid(), pid);
            if (n > 1) { // si on n'est pas au dernier niveau souhaité de l'arbre :
                create_two_children(n - 1, getpid()); // création de deux processus fils à celui courant, 
                                     // par récursivité et en mettant à jour le niveau de l'arbre.
            }
            sleep(1);
            exit(0);

        }
    }
    // boucle pour attendre la fin des processus fils :
    for (i = 0; i < 2; i++) {
        sleep(5); // pour avoir le temps de faire pstree -p dans la console notamment.
        wait(NULL);
    }
}

int main(int argc, char *argv[]) {
    
    // Vérification du nombre d'arguments :
    if (argc != 2) {
        printf("Argument manquant: %s <n>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);  // convertit l'argument en entier.

    pid_t start_pid = getpid();  // PID du pid du processus de départ.
    printf("PID du process de départ : %d.\n", start_pid);

    create_two_children(n, start_pid);  // création de l'arbre de process.

    return 0;
}