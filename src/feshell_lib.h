#include "./parse_lib/parse_lib.h"

#ifndef _feshell_lib
#define _feshell_lib

#define MAX_DIM_BUFF 1024

void shellInfo();
int countTokens(char *, char *);
int cd(char **);
void execute(int, char **);
void create_pipes(cmd_t *, int);

int *pipes;
int pipe_index;

#endif
