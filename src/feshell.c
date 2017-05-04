#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int pid, status;
    char buff[1024];
    char *cmd;
    char **args = NULL;
    int n_args;

    printf("> ");
    while (fgets(buff, 1025, stdin) != NULL) {
        args = NULL;
        n_args = 0;

        if (!strcmp(buff, "\n")) {
            printf("> ");
            continue;
        }
        else if (!strcmp(buff, "clear")) {
            system("clear");
            continue;
        }

        cmd = strtok(buff, " \t\n;");

        if (strstr(cmd, "exit") != NULL) exit(0);

        while (cmd != NULL) {
            args = realloc(args, sizeof (char*) * ++n_args);

            if (args == NULL) exit (1);

            if (strlen(cmd)) {
                args[n_args - 1] = cmd;
            }

            cmd = strtok(NULL, " \t\n;");
        }

        if (n_args) {
            pid = fork();

            if (pid == 0) {
                if (execvp(args[0], args) == -1) {
                    fprintf(stderr, "-feshell: %s: ", args[0]);
                    perror("");
                }
            } else if (pid > 0) {
                pid = wait(&status);
                /*
                 * gestione dello stato
                 */
            } else {
                printf("fork fallita");
            }
        }

        printf("> ");
        fflush(stdin);
    }

    if (feof(stdin)) {
        printf("exit");
        kill(getppid(), SIGINT);
    }

    free(args);

    return 0;
}
