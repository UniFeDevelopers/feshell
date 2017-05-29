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
        if (execvp(*args, args) == -1) {
            fprintf(stderr, "-feshell: %s: ", *args);
            perror("");
        }
    }
}

void fork_pipes(int n, cmd_t *list) {
    int i, j, status;
    //int fd;
    pid_t pid;

    cmd_t *tmp = list;

    int *pipes = (int *) malloc(sizeof(int) * 2 * (n - 1));


    // *** primo processo ***

    pipe(pipes);

    if ((pid = fork()) == 0) {
        if (dup2(pipes[1], 1) == -1) {
            fprintf(stderr, "-feshell: Errore  pipe: i = 0, pipe[1)]");
            perror("");
        }

        close(pipes[0]);
        close(pipes[1]);

        execute(tmp->n_args, tmp->args);
    }

    tmp = tmp->next;


    // *** processi intermedi ***

    for (i = 1; i < n - 1; i++) {
        pipe(pipes + 2 * i);

        if ((pid = fork()) == 0) {
            /*
            if (tmp->node_type == 1) {
                //pipes[1] = open(tmp->nome, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                fd = open(tmp->nome, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                dup2(pipes[1], fd);
                close(pipes[0]);
                close(pipes[1]);
            }
            else if (tmp->node_type == 2) {
                fd = open(tmp->nome, O_RDWR | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
                dup2(pipes[1], fd);
                close(pipes[0]);
                close(pipes[1]);
            }
            */

            if (dup2(pipes[2 * (i - 1)], 0) == -1) {
                fprintf(stderr, "-feshell: Errore  pipe: i = %d, pipe[2 * (i - 1)]", i);
                perror("");
            }

            if (dup2(pipes[(2 * i) + 1], 1) == -1) {
                fprintf(stderr, "-feshell: Errore  pipe: i = %d, pipe[2 * i + 1]", i);
                perror("");
            }

            for (j = 0; j <= (2 * i) + 1; j++) {
                close(pipes[j]);
            }

            //close(pipes[2 * (i - 1)]);
            //close(pipes[2 * (i - 1) + 1]);
            //close(pipes[(2 * i)]);
            //close(pipes[(2 * i) + 1]);

            execute(tmp->n_args, tmp->args);
        }

        tmp = tmp->next;
    }


    // *** ultimo processo ***

    if ((pid = fork()) == 0) {
        if (dup2(pipes[2 * (n - 2)], 0) == -1) {
            fprintf(stderr, "-feshell: Errore  pipe: i = last, pipe[2 * (n - 2)]");
            perror("");
        }

        for (j = 0; j <= 2 * (n - 2) + 1; j++) {
            close(pipes[j]);
        }

        //close(pipes[2 * (n - 2)]);
        //close(pipes[2 * (n - 2) + 1]);

        execute(tmp->n_args, tmp->args);
    }


    // chiude tutte le pipe
    for (i = 0; i < 2 * (n - 1); i++) {
        close(pipes[i]);
    }

    // aspetta che tutti i figli termino
    for (i = 0; i < n; i++) {
        wait(&status);
    }

    free(pipes);
}
