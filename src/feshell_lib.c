#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>

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
    if (!strcmp(*args, "cd")) {
        cd(args);
    }
    else if (!strcmp(*args, "ls")) {
        list_dir(n_args, args);
    }
    else {
        int i;
        char **exec_args;

        exec_args = (char **) malloc(sizeof(char *) * (n_args + 1));

        for (i = 0; i < n_args; i++) {
            exec_args[i] = (char *) malloc(sizeof(char) * (strlen(args[i]) + 1));
            strcpy(exec_args[i], args[i]);
        }
        exec_args[i] = NULL;

        if (execvp(*args, args) == -1) {
            fprintf(stderr, "-feshell: %s: ", *args);
            perror("");
        }

        free(exec_args);
    }
}

void exec_proc(int in, int out, cmd_t *cmd) {
    pid_t pid;

    if ((pid = fork()) == 0) {
        if (in != 0) {
            dup2(in, 0);
            close(in);
        }

        if (out != 1) {
            dup2(out, 1);
            close(out);
        }

        execute(cmd->n_args, cmd->args);
    }
}

void fork_pipes(int n, cmd_t *list) {
    int i;
    int in, pipes[2];
    cmd_t *tmp;

    tmp = list;

    in = 0;

    for (i = 0; i < n - 1; i++) {
        pipe(pipes);

        if (tmp->node_type == 1) {
            pipes[1] = open(tmp->nome, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        }
        else if (tmp->node_type == 2) {
            pipes[1] = open(tmp->nome, O_RDWR | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
        }

        exec_proc(in, pipes[1], tmp);
        close(pipes[1]);

        in = pipes[0];

        tmp = tmp->next;
    }

    if (!in) {
        dup2(in, 0);
    }

    execute(tmp->n_args, tmp->args);
}
