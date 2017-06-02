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
            printf("- el->node_type:\t");
            switch (tmp->node_type) {
                case 0:
                    printf("%d -> exec\n", tmp->node_type);
                    break;
                case 1:
                    printf("%d -> read\n", tmp->node_type);
                    break;
                case 2:
                    printf("%d -> write\n", tmp->node_type);
                    break;
                case 3:
                    printf("%d -> read write\n", tmp->node_type);
                    break;
                case 4:
                    printf("%d -> append\n", tmp->node_type);
                    break;
                case 5:
                    printf("%d -> read append\n", tmp->node_type);
                    break;
                default:
                    fprintf(stderr, "ERRORE NODE TYPE: %d\n", tmp->node_type);
                    break;
            }

            printf("- el->nome:     \t%s\n", tmp->nome);
            printf("- el->n_childs: \t%d\n", tmp->n_childs);
            printf("- el->fileIn:   \t%s\n", tmp->fileIn);
            printf("- el->fileOut:  \t%s\n", tmp->fileOut);

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
