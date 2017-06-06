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

void appendElement(cmd_t **cmd_list, cmd_t *el) {
    if (*cmd_list == NULL) {
        el->n_childs = 1;
        *cmd_list = el;
    }
    else {
        cmd_t *head;

        head = *cmd_list;
        head->n_childs++;

        while (head->next != NULL) {
            head = head->next;
        }
        head->next = el;
    }
}

void createElement(cmd_t **cmd_list, char **args, int n_args, char *fileIn, char *fileOut, int type) {
    int i;
    cmd_t *new;

    new = (cmd_t *) malloc(sizeof(cmd_t));

    new->node_type = type;
    strcpy(new->nome, args[0]);
    for (i = 0; i < n_args; i++) {
        new->args[i] = (char *) malloc(sizeof(char) * strlen(args[i]));
        strcpy(new->args[i], args[i]);
    }
    new->n_args = n_args;
    strcpy(new->fileIn, fileIn);
    strcpy(new->fileOut, fileOut);
    new->next = NULL;
    new->n_childs = 0;

    appendElement(cmd_list, new);
}

cmd_t *parse(char *buff) {
    cmd_t *cmd_list;
    cmd_list = NULL;

    char buff_copy[MAX_DIM_BUFF];
    char *cmd = NULL;
    char **args = NULL;
    char fileIn[STRING_MAX_L] = "";
    char fileOut[STRING_MAX_L] = "";
    int n_args = 0;
    int tokens;
    int i;

    int is_file_out = 0;
    int is_file_append = 0;
    int is_file_in = 0;
    int type = 0;

    strcpy(buff_copy, buff);
    tokens = countTokens(buff_copy, " \t\n;");

    if (!tokens) {
        return NULL;
    }

    args = (char **) malloc(sizeof(char *) * (tokens + 1));
    if (args == NULL) exit(1);

    cmd = strtok(buff, " \t\n;");
    for (i = 0; i < tokens; i++) {
        if (strlen(cmd) && strcmp(cmd, "\t") && strcmp(cmd, " ")) {
            if (!strcmp(cmd, "|")) {
                createElement(&cmd_list, args, n_args, fileIn, fileOut, type);
                type = 0;

                is_file_in = is_file_in ? 0 : is_file_in;

                args = (char **) malloc(sizeof(char *) * (tokens + 1));
                n_args = 0;

                is_file_in = is_file_out = is_file_append = 0;
            }
            else if (!strcmp(cmd, ">")) {
                is_file_out = 1;
            }
            else if (!strcmp(cmd, ">>")) {
                is_file_append = 1;
            }
            else if (!strcmp(cmd, "<")) {
                is_file_in = 1;
            }
            else {
                if (!(is_file_in || is_file_out || is_file_append))  {
                    args[n_args] = (char *) malloc(sizeof(char) * (STRING_MAX_L + 1));
                    strcpy(args[n_args], cmd);
                    n_args++;
                }

                if (is_file_in) {
                    strcpy(fileIn, cmd);
                    type += 1;
                    is_file_in = 0;
                }

                if (is_file_out) {
                    strcpy(fileOut, cmd);
                    type += 2;
                    is_file_out = 0;
                }
                else if (is_file_append) {
                    strcpy(fileOut, cmd);
                    type += 4;
                    is_file_append = 0;
                }
            }
        }

        cmd = strtok(NULL, " \t\n;");
    }

    createElement(&cmd_list, args, n_args, fileIn, fileOut, type);

    free(args);
    free(cmd);

    return cmd_list;
}
