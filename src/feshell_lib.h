#include "./parse_lib/parse_lib.h"

#ifndef _feshell_lib
#define _feshell_lib

#define MAX_DIM_BUFF 1024

void shellInfo();
int cd(char **);
void fork_pipes(int, cmd_t *);

#endif
