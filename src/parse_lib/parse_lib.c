#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include "../feshell_lib.h"
#include "./parse_lib.h"

void appendCoda(cmd_t **cmd_list, cmd_t *el) {
    cmd_t *head;

    head = *cmd_list;
    while (head->next != NULL) {
        head = head->next;
    }
    head->next = el;
}

int countTokens(char *str, char *limit) {
    int count = 0;
    char *token;

    if (str != NULL && strlen(str)) {
        token = strtok(str, limit);

        while (token != NULL) {
            count++;
            token = strtok(NULL, limit);
        }
    }

    return count;
}

void appendTesta(cmd_t **cmd_list, cmd_t *el) {
    el->next = *cmd_list;
    *cmd_list = el;
}

void appendElement(cmd_t **cmd_list, cmd_t *el, int mode) {
    if (*cmd_list == NULL) {
        *cmd_list = el;
    }
    else {
        if (mode) {
            appendCoda(cmd_list, el);
        }
        else {
            appendTesta(cmd_list, el);
        }
    }
}

int parse(char *buff) {
    char buff_copy[MAX_DIM_BUFF];

    strcpy(buff_copy, buff);

    // 1- separa per <, >, >>, |
    // 2- manda a tokenize_n_push
    // + crea lista e passala a t_n_p


    return 0;
}

void tokenize_n_push(char *buff, /*LISTA*/) {
    char buff_copy[MAX_DIM_BUFF];
    char *cmd = NULL;
    char **args = NULL;
    int n_args = 0;
    int tokens;
    int i;

    strcpy(buff_copy, buff);
    tokens = countTokens(buff_copy, " \t\n;");

    args = (char **) malloc(sizeof(char *) * (tokens + 1));
    if (args == NULL) exit(1);

    cmd = strtok(buff, " \t\n;");
    for (i = 0; i < tokens; i++) {
        if (strlen(cmd) && strcmp(cmd, "\t") && strcmp(cmd, " ")) {
            args[i] = (char *) malloc(sizeof(char) * (strlen(cmd) + 1));
            strcpy(args[i], cmd);
            n_args++;

            if (!strcmp(args[i], "|") || !strcmp(args[i], ">>") || !strcmp(args[i], ">")) {
                // crea struct

                args = NULL;
                n_args = 0;
            }
            else if (!strcmp(args[i], "<")) {
                // crea struct

                args = NULL;
                n_args = 0;
            }
        }

        cmd = strtok(NULL, " \t\n;");
    }

    args[i] = NULL;

    if (n_args) {
        /*
        if (execute(n_args, args) == 1) {
            exit(1);
        }
        */

        // 3- crea struct e lista, pusha sulla lista
    }

    free(args);
    free(cmd);
}
