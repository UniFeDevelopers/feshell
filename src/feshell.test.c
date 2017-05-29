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
    cmd_t *cmd_list;
    cmd_t *tmp;
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
            exit(0);
        }

        cmd_list = parse(buff);

        tmp = cmd_list;
        printf("\nLISTA:\n\n");

        while (tmp != NULL) {
            printf("- el->node_type:\t%i\n", tmp->node_type);
            printf("- el->nome:     \t%s\n", tmp->nome);
            printf("- el->n_childs: \t%d\n", tmp->n_childs);
            printf("- el->args (%d):\n", tmp->n_args);
            for (i = 0; i < tmp->n_args; i++) {
                printf("           \t%s\n", tmp->args[i]);
            }

            printf("\n");
            tmp = tmp->next;
        }

        shellInfo();
    }

    if (feof(stdin)) {
        printf("exit\n");
    }

    return 0;
}
