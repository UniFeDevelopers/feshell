#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#include "./ls/ls_lib.h"
#include "./feshell_lib.h"

void shellInfo() {
    char hostn[1024] = "";
    char currentDirectory[1024] = "";
    gethostname(hostn, sizeof(hostn));
    if (strstr(hostn, "local") != NULL) {
        hostn[strlen(hostn) - 6] = '\0';
    }
    printf("\x1b[1m\x1B[32m%s@%s\x1b[0m:\x1b[1m\x1B[34m%s \x1b[0m$ ", getenv("LOGNAME"), hostn, strrep(getcwd(currentDirectory, 1024), getenv("HOME"), "~"));
}

int countTokens(char *str, char *limit) {
    int count = 0;
    char *token;

    if (str != NULL && strlen(str)) {
        token = strtok(str, limit);

        while (token != NULL) {
            count++;
            token = strtok(NULL, limit);
        }
    }

    return count;
}

int cd(char *args[]) {
    if (args[1] == NULL) {
        chdir(getenv("HOME"));
        return 0;
    }
    else{
        if (chdir((const char *) strrep(args[1], "~", getenv("HOME"))) == -1) {
            fprintf(stderr, "-feshell: %s: %s: ", args[0], args[1]);
            perror("");
            return 1;
        }
    }
    return 0;
}

int execute(int n_args, char *args[]) {
    int pid, status;

    if (strstr(args[0], "cd") != NULL) {
        return cd(args);
    }

    pid = fork();

    if (pid == 0) {
        if (!strcmp(args[0], "ls")) {
            list_dir(n_args, args);
            return 0;
        }
        else {
            if (execvp(args[0], args) == -1) {
                fprintf(stderr, "-feshell: %s: ", args[0]);
                perror("");
            }
            exit(EXIT_FAILURE);
            return 0;
        }
    } else if (pid > 0) {
        pid = wait(&status);
        /*
         * gestione dello stato
         * wait return child process exit value
         */
    } else {
        fprintf(stderr, "-feshell: fork fallita");
        return 1;
    }

    return 0;
}
