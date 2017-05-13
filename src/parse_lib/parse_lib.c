#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

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
    while (head->next != NULL) {
        head = head->next;
    }
    head->next = el;
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
    cmd_t *cmd_list, *tmp;
    cmd_list = NULL;

    tokenize_n_push(buff, &cmd_list);

    if (cmd_list == NULL) return 1;

    tmp = cmd_list;
    char **exec_args;
    int status;
    int i;

    while (tmp != NULL) {
        exec_args = (char **) malloc(sizeof(char *) * (tmp->n_args + 1));
        for (i = 0; i < tmp->n_args; i++) {
            exec_args[i] = (char *) malloc(sizeof(char *) * strlen(tmp->args[i]));
            strcpy(exec_args[i], tmp->args[i]);
        }
        exec_args[i] = NULL;

        if (fork() == 0) {
            if (execvp(exec_args[0], exec_args) == -1) {
                fprintf(stderr, "-feshell: %s: ", exec_args[0]);
                perror("");
                exit(1);
            }
            exit(0);
        }
        else {
            wait(&status);
        }
        tmp = tmp->next;
    }

    return 0;
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

                new->node_type = 1;
                strcpy(new->nome, args[0]);
                for (j = 0; j < n_args; j++) {
                    new->args[j] = (char *) malloc(sizeof(char) * strlen(args[j]));
                    strcpy(new->args[j], args[j]);
                }
                new->n_args = n_args;
                new->next = NULL;

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
    args[j] = NULL;

    appendElement(lista, new, 1);

    free(args);
    free(cmd);
}
