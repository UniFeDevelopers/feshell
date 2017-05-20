#ifndef _cmd
#define _cmd

#define STRING_MAX_L 1024

typedef struct _cmd_t {
    int node_type;              // 0 = exec, 1 = file write, 2 = file append
    char nome[STRING_MAX_L];
    char *args[STRING_MAX_L];
    int n_args;
    int n_childs;

    struct _cmd_t *next;
} cmd_t;

// 0 per < , 1 per tutti gli altri casi
void appendElement(cmd_t **, cmd_t *, int);
cmd_t *parse(char *);
int countTokens(char *, char *);
void tokenize_n_push(char *, cmd_t **);

#endif
