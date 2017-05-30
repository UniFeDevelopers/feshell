#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>

#include "./feshell_lib.h"
#include "./parse_lib/parse_lib.h"

int main(void) {
    char buff[MAX_DIM_BUFF];
    char buff_copy[MAX_DIM_BUFF];
    cmd_t *cmd_list;
    int pid, status;
    int n_cmds;

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
            if ((pid = fork()) == 0) {
                execute(cmd_list->n_args, cmd_list->args);
                exit(0);
            }
            else {
                wait(&status);
            }
        }

        shellInfo();
    }

    if (feof(stdin)) {
        printf("exit\n");
    }

    return 0;
}
