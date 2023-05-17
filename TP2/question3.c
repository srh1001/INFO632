#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();
    if (pid == -1) {
        printf("Erreur lors de la création du processus fils.\n");
        exit(1);
    } else if (pid == 0) { // Process fils
        sleep(1);
        exit(0);
    } else { // Process parent
        printf("Process parent %d a créé le process fils %d.\n", getpid(), pid);
        sleep(10); // Le process parent dort pendant 10s => le process fils est zombie pendant 10s.
        wait(NULL); // Process parent récupère le statut du fils une fois celui-ci fini.
        printf("Le process parent a récupéré le statut du fils, ce dernier n'est plus un zombie.\n");
        exit(0);
    }
}