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
    if (!strcmp(*args, "ls")) {
        list_dir(n_args, args);
    }
    else {
        if (execvp(*args, args) == -1) {
            fprintf(stderr, "-feshell: %s: ", *args);
            perror("");
            exit(1);
        }
    }
}

void fork_pipes(int n, cmd_t *list) {
    int i, j, status;
    pid_t pid;

    cmd_t *tmp = list;

    int *pipes = (int *) malloc(sizeof(int) * 2 * (n - 1));
    int in, out;
    int usedIn = 0, usedOut = 0;


    // *** primo processo ***

    if (n > 1) {
        if (pipe(pipes) == -1) {
            fprintf(stderr, "-feshell: failed while creating pipes for: %s", tmp->args[0]);
            perror("");
            exit(1);
        }
    }

    if (tmp->node_type == 1 || tmp->node_type == 3 || tmp->node_type == 5) {
        in = open(tmp->fileIn, O_RDONLY);
        usedIn = 1;

        if (in < 0) {
            fprintf(stderr, "-feshell: Error opening: %s\n", tmp->fileIn);
            perror("");
            exit(1);
        }
    }

    if (tmp->node_type == 2 || tmp->node_type == 3) {
        out = open(tmp->fileOut, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        usedOut = 1;
        if (out < 0) {
            fprintf(stderr, "-feshell: Error opening: %s\n", tmp->fileIn);
            perror("");
            exit(1);
        }
    }

    if (tmp->node_type == 4 || tmp->node_type == 5) {
        out = open(tmp->fileOut, O_RDWR | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        usedOut = 1;
        if (out < 0) {
            fprintf(stderr, "-feshell: Error opening: %s\n", tmp->fileIn);
            perror("");
            exit(1);
        }
    }

    pid = fork();
    if (pid == 0) {
        if (tmp->node_type <= 1 && list->n_childs > 1) {
            if (dup2(pipes[1], 1) == -1) {
                fprintf(stderr, "-feshell: Errore  pipe: i = 0, pipe[1]");
                perror("");
                exit(1);
            }

            close(pipes[0]);
            close(pipes[1]);
        }

        if (tmp->node_type == 1 || tmp->node_type == 3 || tmp->node_type == 5) {
            if (dup2(in, 0) == -1) {
                fprintf(stderr, "-feshell: Error reading from pipe: %s\n", tmp->fileIn);
                perror("");
                exit(1);
            }
        }

        if (tmp->node_type == 2 || tmp->node_type == 3) {
            if (dup2(out, 1) == -1) {
                fprintf(stderr, "-feshell: Error writing to pipe: %s\n", tmp->fileIn);
                perror("");
                exit(1);
            }
        }

        if (tmp->node_type == 4 || tmp->node_type == 5) {
            if (dup2(out, 1) == -1) {
                fprintf(stderr, "-feshell: Error writing to pipe: %s\n", tmp->fileIn);
                perror("");
                exit(1);
            }
        }

        if (tmp->node_type == 1 || tmp->node_type == 3 || tmp->node_type == 5) {
            close(in);
        }

        if (tmp->node_type > 1) {
            close(out);
        }

        execute(tmp->n_args, tmp->args);
        exit(0);
    }
    else if (pid < 0) {
        fprintf(stderr, "-feshell: fork failed for: %s", tmp->args[0]);
        perror("");
        exit(1);
    }

    if (n > 1) {
        tmp = tmp->next;

        // *** processi intermedi ***

        for (i = 1; i < n - 1; i++) {
            if (pipe(pipes + 2 * i) == -1) {
                fprintf(stderr, "-feshell: failed while creating pipes for: %s", tmp->args[0]);
                perror("");
                exit(1);
            }

            pid = fork();
            if (pid == 0) {

                if (dup2(pipes[2 * (i - 1)], 0) == -1) {
                    fprintf(stderr, "-feshell: Errore  pipe: i = %d, pipe[2 * (i - 1)]", i);
                    perror("");
                    exit(1);
                }

                if (dup2(pipes[(2 * i) + 1], 1) == -1) {
                    fprintf(stderr, "-feshell: Errore  pipe: i = %d, pipe[2 * i + 1]", i);
                    perror("");
                    exit(1);
                }

                for (j = 0; j <= (2 * i) + 1; j++) {
                    close(pipes[j]);
                }

                execute(tmp->n_args, tmp->args);
                exit(0);
            }
            else if (pid < 0) {
                fprintf(stderr, "-feshell: fork failed for: %s", tmp->args[0]);
                perror("");
                exit(1);
            }

            tmp = tmp->next;
        }


        // *** ultimo processo ***

        if (tmp->node_type == 2) {
            out = open(tmp->fileOut, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            usedOut = 1;
            if (out < 0) {
                fprintf(stderr, "-feshell: Error opening: %s\n", tmp->fileIn);
                perror("");
                exit(1);
            }
        }

        if (tmp->node_type == 4) {
            out = open(tmp->fileOut, O_RDWR | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            usedOut = 1;
            if (out < 0) {
                fprintf(stderr, "-feshell: Error opening: %s\n", tmp->fileIn);
                perror("");
                exit(1);
            }
        }

        pid = fork();
        if (pid == 0) {
            if (dup2(pipes[2 * (n - 2)], 0) == -1) {
                fprintf(stderr, "-feshell: Errore  pipe: i = last, pipe[2 * (n - 2)]");
                perror("");
                exit(1);
            }

            for (j = 0; j <= 2 * (n - 2) + 1; j++) {
                close(pipes[j]);
            }

            if (tmp->node_type == 2) {
                if (dup2(out, 1) == -1) {
                    fprintf(stderr, "-feshell: Error writing to pipe: %s\n", tmp->fileIn);
                    perror("");
                    exit(1);
                }
            }

            if (tmp->node_type == 4) {
                if (dup2(out, 1) == -1) {
                    fprintf(stderr, "-feshell: Error writing to pipe: %s\n", tmp->fileIn);
                    perror("");
                    exit(1);
                }
            }

            if (tmp->node_type == 2 || tmp->node_type == 4) {
                close(out);
            }

            execute(tmp->n_args, tmp->args);
            exit(0);
        }
        else if (pid < 0) {
            fprintf(stderr, "-feshell: fork failed for: %s", tmp->args[0]);
            perror("");
            exit(1);
        }


        // chiude tutte le pipe
        for (i = 0; i < 2 * (n - 1); i++) {
            close(pipes[i]);
        }
    }

    // chiude i fd per le redirect
    if (usedIn) {
        close(in);
    }
    if (usedOut) {
        close(out);
    }

    // aspetta che tutti i figli termino
    for (i = 0; i < n; i++) {
        wait(&status);
    }

    free(pipes);
}
