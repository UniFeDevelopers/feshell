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
    cmd_t *cmd_list;

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
            exit(0);
        }

        cmd_list = parse(buff);

        if (!strcmp(cmd_list->args[0], "cd")) {
            cd(cmd_list->args);
        }
        else {
            fork_pipes(cmd_list->n_childs, cmd_list);
        }

        shellInfo();
    }

    if (feof(stdin)) {
        printf("exit\n");
    }

    return 0;
}
