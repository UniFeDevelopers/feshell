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
        if (mode == CODA) {
            appendCoda(cmd_list, el);
        }
        else {
            appendTesta(cmd_list, el);
        }
    }
}

void createElement(cmd_t **cmd_list, char **args, int n_args, int is_file_in, int is_file_out) {
    int i;
    cmd_t *new;

    new = (cmd_t *) malloc(sizeof(cmd_t));

    new->node_type = is_file_out;
    strcpy(new->nome, args[0]);
    for (i = 0; i < n_args; i++) {
        new->args[i] = (char *) malloc(sizeof(char) * strlen(args[i]));
        strcpy(new->args[i], args[i]);
    }
    new->n_args = n_args;
    new->next = NULL;
    new->n_childs = 0;

    appendElement(cmd_list, new, is_file_in ? TESTA : CODA);
}

cmd_t *parse(char *buff) {
    cmd_t *cmd_list;
    cmd_list = NULL;

    char buff_copy[MAX_DIM_BUFF];
    char *cmd = NULL;
    char **args = NULL;
    int n_args = 0;
    int tokens;
    int i;

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
                createElement(&cmd_list, args, n_args, is_file_in, is_file_out);

                is_file_in = is_file_in ? 0 : is_file_in;

                args = (char **) malloc(sizeof(char *) * (tokens + 1));
                n_args = 0;

                is_file_out = 0;
            }
            else if (!strcmp(cmd, ">")) {
                createElement(&cmd_list, args, n_args, is_file_in, is_file_out);

                is_file_in = is_file_in ? 0 : is_file_in;

                args = (char **) malloc(sizeof(char *) * (tokens + 1));
                n_args = 0;

                is_file_out = 1;
            }
            else if (!strcmp(cmd, ">>")) {
                createElement(&cmd_list, args, n_args, is_file_in, is_file_out);

                is_file_in = is_file_in ? 0 : is_file_in;

                args = (char **) malloc(sizeof(char *) * (tokens + 1));
                n_args = 0;

                is_file_out = 2;
            }
            else if (!strcmp(cmd, "<")) {
                createElement(&cmd_list, args, n_args, is_file_in, is_file_out);

                is_file_in = is_file_in ? 0 : is_file_in;

                args = (char **) malloc(sizeof(char *) * (tokens + 1));
                n_args = 0;

                is_file_in = 1;
                is_file_out = 0;
            }
            else {
                if (is_file_in) {
                    args[0] = "cat";
                    args[1] = (char *) malloc(sizeof(char) * (STRING_MAX_L + 1));
                    strcpy(args[1], cmd);
                    n_args = 2;
                }
                else {
                    args[n_args] = (char *) malloc(sizeof(char) * (STRING_MAX_L + 1));
                    strcpy(args[n_args], cmd);
                    n_args++;
                }
            }
        }

        cmd = strtok(NULL, " \t\n;");
    }

    createElement(&cmd_list, args, n_args, is_file_in, is_file_out);

    free(args);
    free(cmd);

    return cmd_list;
}
