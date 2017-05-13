#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include "./feshell_lib.h"
#include "./ls/ls_lib.h"
#include "./parse_lib/parse_lib.h"

void shellInfo() {
    char hostn[MAX_DIM_BUFF] = "";
    char currentDirectory[MAX_DIM_BUFF] = "";
    gethostname(hostn, sizeof(hostn));
    if (strstr(hostn, "local") != NULL) {
        hostn[strlen(hostn) - 6] = '\0';
    }
    printf("\x1b[1m\x1B[32m%s@%s\x1b[0m:\x1b[1m\x1B[34m%s \x1b[0m$ ", getenv("LOGNAME"), hostn, strrep(getcwd(currentDirectory, MAX_DIM_BUFF), getenv("HOME"), "~"));
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

void execute(int n_args, char *args[]) {
    if (!strcmp(args[0], "ls")) {
        list_dir(n_args, args);
    }
    else {
        if (execvp(args[0], args) == -1) {
            fprintf(stderr, "-feshell: %s: ", args[0]);
            perror("");
        }
        exit(EXIT_FAILURE);
    }
}
