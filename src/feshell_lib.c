#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#include "./ls/ls_lib.h"
#include "./feshell_lib.h"

int cd(char *args[]) {
    if (args[1] == NULL) {
        chdir(getenv("HOME"));
        return 0;
    }
    else{
        if (chdir(args[1]) == -1) {
            fprintf(stderr, "-feshell: %s: %s: ", args[0], args[1]);
            perror("");
            return 1;
        }
    }
    return 0;
}

int execute(char *args[]) {
    int pid, status;

    if (strstr(args[0], "cd") != NULL) {
        return cd(args);
        //shellInfo();
    }

    pid = fork();

    if (pid == 0) {
        if (!strcmp(args[0], "ls")) {
            balorda();
            return 0;
        }
        else {
            if (execvp(args[0], args) == -1) {
                fprintf(stderr, "-feshell: %s: command not found\n", args[0]);
                return 1;
            }
            return 0;
        }
    } else if (pid > 0) {
        pid = wait(&status);
        /*
         * gestione dello stato
         */
    } else {
        fprintf(stderr, "-feshell: fork fallita");
        return 1;
    }

    return 0;
}
