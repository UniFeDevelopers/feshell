#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include "./feshell_lib.h"

int main(int argc, char *argv[]) {
    char buff[1024];
    char *cmd;
    char **args = NULL;
    int n_args;

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

        cmd = strtok(buff, " \t\n;");

        while (cmd != NULL) {
            if (strstr(cmd, "exit") != NULL) {
                exit(0);
            }

            args = (char **) realloc(args, sizeof (char*) * ++n_args);

            if (args == NULL) exit(1);

            if (strlen(cmd) && strcmp(cmd, "\t") && strcmp(cmd, " ")) {
                args[n_args - 1] = cmd;
            }

            cmd = strtok(NULL, " \t\n;");
        }

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
