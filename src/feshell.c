#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include "./feshell_lib.h"
#include "./parse_lib/parse_lib.h"

int main(void) {
    char buff[MAX_DIM_BUFF];
    cmd_t *cmd_list;
    char **exec_args;
    int i;

    shellInfo();
    while (fgets(buff, MAX_DIM_BUFF + 1, stdin) != NULL) {
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
            pipes = (int *) malloc(sizeof(int) * cmd_list->n_childs);
            create_pipes(cmd_list, 1);

            for (i = 0; i < MAX_DIM_BUFF; i++) {
                close(pipes[i]);
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

            execute(cmd_list->n_args, exec_args);
        }

        shellInfo();
        fflush(stdin);
    }

    if (feof(stdin)) {
        printf("exit");
        kill(getppid(), SIGINT);
    }

    return 0;
}
