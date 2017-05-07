#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include "./feshell_lib.h"

int main(int argc, char *argv[]) {
    char buff[1024];
    char buff_copy[1024];
    char *cmd = NULL;
    char **args = NULL;
    int n_args;
    int tokens;
    int i;

    shellInfo();
    while (fgets(buff, 1025, stdin) != NULL) {
        args = NULL;
        n_args = 0;

        if (!strcmp(buff, "\n") || !strlen(buff)) {
            shellInfo();
            continue;
        }
        else if (!strcmp(buff, "clear")) {
            system("clear");
            continue;
        }

        strcpy(buff_copy, buff);
        tokens = countTokens(buff_copy, " \t\n;");

        args = (char **) malloc(sizeof(char *) * (tokens + 1));
        if (args == NULL) exit(1);

        cmd = strtok(buff, " \t\n;");
        for (i = 0; i < tokens; i++) {
            if (strlen(cmd) && strcmp(cmd, "\t") && strcmp(cmd, " ")) {
                args[i] = (char *) malloc(sizeof(char) * (strlen(cmd) + 1));
                strcpy(args[i], cmd);
                n_args++;
            }

            cmd = strtok(NULL, " \t\n;");
        }

        args[i] = NULL;

        if (n_args) {
            if (execute(n_args, args) == 1) {
                exit(1);
            }
        }

        shellInfo();
        fflush(stdin);
    }

    if (feof(stdin)) {
        printf("exit\n");
        kill(getppid(), SIGINT);
    }

    free(args);
    free(cmd);

    return 0;
}
