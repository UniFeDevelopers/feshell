#ifndef _cmd
#define _cmd

#define STRING_MAX_L 1024
#define TESTA 0
#define CODA 1

/*
 * NODE TYPES *
 *
 * 0:    exec
 * 1:    read
 * 2:    write
 * 3:    read write
 * 4:    append
 * 5:    read append
 */

typedef struct _cmd_t {
    int node_type;
    char nome[STRING_MAX_L];
    char *args[STRING_MAX_L];
    char fileOut[STRING_MAX_L];
    char fileIn[STRING_MAX_L];
    int n_args;
    int n_childs;

    struct _cmd_t *next;
} cmd_t;

cmd_t *parse(char *);

#endif
