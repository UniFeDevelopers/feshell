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

        n_cmds = cmd_list->n_childs - 1;
        fork_pipes(n_cmds, cmd_list);

        shellInfo();
    }

    if (feof(stdin)) {
        printf("exit\n");
    }

    return 0;
}
