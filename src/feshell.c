#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include "./feshell_lib.h"

void shellInfo() {
    char hostn[1024] = "";
    gethostname(hostn, sizeof(hostn));
    hostn[strlen(hostn) - 6] = '\0';
    printf("\x1b[1m\x1B[32m%s@%s\x1b[0m:\x1b[1m\x1B[34m%s \x1b[0m$ ", getenv("LOGNAME"), hostn, getenv("PWD"));
}

int main(int argc, char *argv[]) {
    char buff[1024];
    char *cmd;
    char **args = NULL;
    int n_args;

    shellInfo();
    while (fgets(buff, 1025, stdin) != NULL) {
        args = NULL;
        n_args = 0;

        if (!strcmp(buff, "\n")) {
            shellInfo();
            continue;
        }
        else if (!strcmp(buff, "clear")) {
            system("clear");
            continue;
        }

        cmd = strtok(buff, " \t\n;");

        if (strstr(cmd, "exit") != NULL) {
            exit(0);
        }

        while (cmd != NULL) {
            args = realloc(args, sizeof (char*) * ++n_args);

            if (args == NULL) exit(1);

            if (strlen(cmd)) {
                args[n_args - 1] = cmd;
            }

            cmd = strtok(NULL, " \t\n;");
        }

        if (n_args) {
            execute(n_args, args);
        }

        shellInfo();
        fflush(stdin);
    }

    if (feof(stdin)) {
        printf("exit\n");
        kill(getppid(), SIGINT);
    }

    free(args);

    return 0;
}
