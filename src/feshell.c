#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>

#include "./feshell_lib.h"
#include "./parse_lib/parse_lib.h"

int main(void) {
    char buff[MAX_DIM_BUFF];
    char buff_copy[MAX_DIM_BUFF];
    cmd_t *cmd_list;
    int pid, status;
    int n_cmds;

    int in, out;

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
            exit(0);
        }

        cmd_list = parse(buff);

        n_cmds = cmd_list->n_childs;

        if (n_cmds > 1) {
            fork_pipes(n_cmds, cmd_list);
        }
        else if (!strcmp(cmd_list->args[0], "cd")) {
            cd(cmd_list->args);
        }
        else {
            if (cmd_list->node_type == 1 || cmd_list->node_type == 3 || cmd_list->node_type == 5) {
                in = open(cmd_list->fileIn, O_RDONLY);

                if (in < 0) {
                    fprintf(stderr, "-feshell: Error opening: %s\n", cmd_list->fileIn);
                    perror("");
                    exit(1);
                }
            }

            if (cmd_list->node_type == 2 || cmd_list->node_type == 3) {
                out = open(cmd_list->fileOut, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
                if (out < 0) {
                    fprintf(stderr, "-feshell: Error opening: %s\n", cmd_list->fileIn);
                    perror("");
                    exit(1);
                }
            }

            if (cmd_list->node_type == 4 || cmd_list->node_type == 5) {
                out = open(cmd_list->fileOut, O_RDWR | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
                if (out < 0) {
                    fprintf(stderr, "-feshell: Error opening: %s\n", cmd_list->fileIn);
                    perror("");
                    exit(1);
                }
            }

            pid = fork();
            if (pid == 0) {
                if (cmd_list->node_type == 1 || cmd_list->node_type == 3 || cmd_list->node_type == 5) {
                    if (dup2(in, 0) == -1) {
                        fprintf(stderr, "-feshell: Error reading from pipe: %s\n", cmd_list->fileIn);
                        perror("");
                        exit(1);
                    }
                }

                if (cmd_list->node_type == 2 || cmd_list->node_type == 3) {
                    if (dup2(out, 1) == -1) {
                        fprintf(stderr, "-feshell: Error writing to pipe: %s\n", cmd_list->fileIn);
                        perror("");
                        exit(1);
                    }
                }

                if (cmd_list->node_type == 4 || cmd_list->node_type == 5) {
                    if (dup2(out, 1) == -1) {
                        fprintf(stderr, "-feshell: Error writing to pipe: %s\n", cmd_list->fileIn);
                        perror("");
                        exit(1);
                    }
                }

                if (cmd_list->node_type % 2 != 0) {
                    close(in);
                }

                if (cmd_list->node_type > 1) {
                    close(out);
                }

                execute(cmd_list->n_args, cmd_list->args);
                exit(0);
            }
            else if (pid < 0) {
                fprintf(stderr, "-feshell: fork failed for: %s", cmd_list->args[0]);
                perror("");
            }

            if (cmd_list->node_type % 2 != 0) {
                close(in);
            }

            if (cmd_list->node_type > 1) {
                close(out);
            }

            wait(&status);
        }

        shellInfo();
    }

    if (feof(stdin)) {
        printf("exit\n");
    }

    return 0;
}
