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
    int i;
    if (!strcmp(*args, "ls")) {
        list_dir(n_args, args);
    }
    else {
        for (i = 0; i < n_args; i++) {
            printf("%d > %s", i, args[i]);
        }

        if (execvp(*args, args) == -1) {
            fprintf(stderr, "-feshell: %s: ", *args);
            perror("");
        }
        exit(EXIT_FAILURE);
    }
}

void create_pipes(cmd_t *list, int isHead) {
    cmd_t *tmp;
    char **exec_args;
    int pid, status;
    int i;

    tmp = list;

    pid = fork();

    if (pid == 0) {
        exec_args = (char **) malloc(sizeof(char *) * (tmp->n_args + 1));
        for (i = 0; i < tmp->n_args; i++) {
            exec_args[i] = (char *) malloc(sizeof(char *) * (strlen(tmp->args[i]) + 1));
            strcpy(exec_args[i], tmp->args[i]);
        }
        exec_args[i] = NULL;

        if (strstr(*exec_args, "cd") != NULL) {
            cd(exec_args);
            return;
        }

        if (tmp->node_type == 0) {
            if (isHead) {                   // primo elemento lista
                dup2(pipes[pipe_index + 1], 1);
            }
            else if (tmp->next == NULL) {   // ultimo elemento lista
                dup2(pipes[pipe_index], 0);
            }
            else {                          // tutti gli altri
                dup2(pipes[pipe_index], 0);
                dup2(pipes[pipe_index + 3], 1);
            }

            for (i = 0; i <= pipe_index + 1; i++) {
                close(pipes[i]);
            }

            if (tmp->next != NULL) {
                create_pipes(tmp->next, 0);
                execute(tmp->n_args, exec_args);
            }
        }
    }
    else if (pid > 0) {
        pipe_index += 2;
        pid = wait(&status);
    }
    else {
        fprintf(stderr, "-feshell: fork fallita");
        return;
    }
}
