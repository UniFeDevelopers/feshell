#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#include "../feshell_lib.h"
#include "./parse_lib.h"

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

void appendCoda(cmd_t **cmd_list, cmd_t *el) {
    cmd_t *head;

    head = *cmd_list;
    head->n_childs++;

    while (head->next != NULL) {
        head = head->next;
    }
    head->next = el;
}

void appendTesta(cmd_t **cmd_list, cmd_t *el) {
    el->next = *cmd_list;
    el->n_childs = el->next->n_childs + 1;
    el->next->n_childs = 0;

    *cmd_list = el;
}

void appendElement(cmd_t **cmd_list, cmd_t *el, int mode) {
    if (*cmd_list == NULL) {
        el->n_childs = 1;
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

cmd_t *parse(char *buff) {
    cmd_t *cmd_list;
    cmd_list = NULL;

    tokenize_n_push(buff, &cmd_list);

    return cmd_list;
}

void tokenize_n_push(char *buff, cmd_t **lista) {
    char buff_copy[MAX_DIM_BUFF];
    char *cmd = NULL;
    char **args = NULL;
    int n_args = 0;
    int tokens;
    int i, j;

    cmd_t *new;

    int is_file_out = 0;
    int is_file_in = 0;

    strcpy(buff_copy, buff);
    tokens = countTokens(buff_copy, " \t\n;");

    args = (char **) malloc(sizeof(char *) * (tokens + 1));
    if (args == NULL) exit(1);

    cmd = strtok(buff, " \t\n;");
    for (i = 0; i < tokens; i++) {
        if (strlen(cmd) && strcmp(cmd, "\t") && strcmp(cmd, " ")) {
            if (!strcmp(cmd, "|")) {
                // crea struct

                new = (cmd_t *) malloc(sizeof(cmd_t));

                new->node_type = 0;
                strcpy(new->nome, args[0]);
                for (j = 0; j < n_args; j++) {
                    new->args[j] = (char *) malloc(sizeof(char) * strlen(args[j]));
                    strcpy(new->args[j], args[j]);
                }
                new->n_args = n_args;
                new->next = NULL;
                new->n_childs = 0;

                appendElement(lista, new, 1);

                args = (char **) malloc(sizeof(char *) * (tokens + 1));
                n_args = 0;
            }
            else if (!strcmp(cmd, ">>") || !strcmp(cmd, ">")) {
                // cose sul file
                is_file_out = 1;        // distingui

                //appendElement(lista, new, 1);
            }
            else if (!strcmp(cmd, "<")) {
                // crea struct
                // cose sul file successivo
                is_file_in = 1;

                //appendElement(lista, new, 0);

                args = (char **) malloc(sizeof(char *) * (tokens + 1));
                n_args = 0;
            }
            else {
                if (is_file_in) {
                    args[0] = "cat";
                    args[1] = (char *) malloc(sizeof(char) * (STRING_MAX_L + 1));
                    strcpy(args[1], cmd);
                }
                else {
                    args[n_args] = (char *) malloc(sizeof(char) * (STRING_MAX_L + 1));
                    strcpy(args[n_args], cmd);
                    n_args++;
                }

                is_file_in = 0;
                is_file_out = 0;
            }
        }

        cmd = strtok(NULL, " \t\n;");
    }

    new = (cmd_t *) malloc(sizeof(cmd_t));

    new->node_type = 1;
    strcpy(new->nome, args[0]);
    for (j = 0; j < n_args; j++) {
        new->args[j] = (char *) malloc(sizeof(char) * strlen(args[j]));
        strcpy(new->args[j], args[j]);
    }
    new->n_args = n_args;
    new->next = NULL;
    new->n_childs = 0;
    args[j] = NULL;

    appendElement(lista, new, 1);

    free(args);
    free(cmd);
}
