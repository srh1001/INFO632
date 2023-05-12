#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork(); // création du processus fils.

    int child_return_value; // initialisation d'une variable qui sera utilisée 
                            // pour stocker la valeur de retour de ce processus fils.
    if (pid == -1) { 
        printf("Erreur lors de la création du processus fils.\n");
        exit(1);
    } else if (pid == 0) { //Si on est dans le processus fils:
        printf("Saisir une valeur à retourner par le processus fils : "); 
        scanf("%d", &child_return_value); // Input de l'utilisateur qui sera stocké 
                                          // dans la variable child_return_value.
        exit(child_return_value); // On quitte le processus fils en retournant la valeur de la variable passée en paramètre.
    } else { // Si on est dans le processus parent:
        wait(&child_return_value); // On attend la fin du processus fils et on stocke sa valeur de retour dans la variable child_return_value.
        printf("Retour du processus fils : %d\n", WEXITSTATUS(child_return_value)); // On affiche la valeur de retour du processus fils.
    }

    return 0;
}