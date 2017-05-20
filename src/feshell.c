#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include "./feshell_lib.h"
#include "./parse_lib/parse_lib.h"

int main(void) {
    char buff[MAX_DIM_BUFF];
    char buff_copy[MAX_DIM_BUFF];
    cmd_t *cmd_list;
    char **exec_args;
    int i, pid, status;

    shellInfo();
    while (fgets(buff, MAX_DIM_BUFF + 1, stdin) != NULL) {
        strcpy(buff_copy, buff);
        if (!strcmp(buff, "\n") || !strlen(buff)) {
            shellInfo();
            continue;
        }
        else if (!strcmp(buff, "clear")) {
            system("clear");
            continue;
        }
        else if (strstr(buff, "exit") != NULL) {
            kill(getppid(), SIGINT);
            exit(0);
        }

        cmd_list = parse(buff);

        if (cmd_list->n_childs > 1) {
            pipe_index = 0;
            n_pipes = cmd_list->n_childs - 1;
            pipes = (int *) malloc(sizeof(int) * 2 * n_pipes);

            for (i = 0; i < n_pipes; i++) {
                pipe(pipes + 2 * i);
            }

            pid = fork();
            if (pid == 0) {
                create_pipes(cmd_list, 1);

                for (i = 0; i < 2 * n_pipes; i++) {
                    close(pipes[i]);
                }
            }
            else if (pid > 0) {
                pid = wait(&status);
            }
            else {
                fprintf(stderr, "-feshell: fork fallita");
                exit(EXIT_FAILURE);
            }
        }
        else {
            exec_args = (char **) malloc(sizeof(char *) * (cmd_list->n_args + 1));
            for (i = 0; i < cmd_list->n_args; i++) {
                exec_args[i] = (char *) malloc(sizeof(char *) * (strlen(cmd_list->args[i]) + 1));
                strcpy(exec_args[i], cmd_list->args[i]);
            }
            exec_args[i] = NULL;

            if (strstr(*exec_args, "cd") != NULL) {
                cd(exec_args);
                shellInfo();
                continue;
            }

            pid = fork();
            if (pid == 0) {
                execute(cmd_list->n_args, exec_args);
            }
            else if (pid > 0) {
                pid = wait(&status);
            }
            else {
                fprintf(stderr, "-feshell: fork fallita");
                exit(EXIT_FAILURE);
            }
        }

        shellInfo();
    }

    if (feof(stdin)) {
        printf("exit");
        kill(getppid(), SIGINT);
    }

    return 0;
}
