#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int pid, status;
    char buff[255];
    char *cmd;
    char **args;
    int n_args;
    int i;

    while (1) {
        printf("> ");
        args = NULL;
        n_args = 0;

        fgets(buff, 256, stdin);
        cmd = strtok(buff, " \n\t;");

        if (strstr(cmd, "exit") != NULL) exit(0);

        while (cmd != NULL) {
            args = realloc(args, sizeof (char*) * ++n_args);

            if (args == NULL) exit (-1);

            if (strlen(cmd)) {
                args[n_args - 1] = cmd;
            }

            cmd = strtok(NULL, " \n\t;");
        }

        printf("\nLISTA COMANDI (%d)\n", n_args);
        for (i = 0; i < n_args; i++) {
            printf("%s - ", args[i]);
        }
        printf("\n");

        // realloc one extra element for the last NULL  (?)
        args = realloc(args, sizeof (char*) * (n_args + 1));
        args[n_args] = 0;

        pid = fork();

        if (pid == 0) {
            if (execvp(args[0], args) == -1) {
                fprintf(stderr, "Exec fallita: %s", args[0]);
            }
            exit(1);
        } else if (pid > 0) {
            pid = wait(&status);
            //<gestione dello stato>
        } else {
            printf("fork fallita");
        }
    }

    free(args);

    return 0;
}
