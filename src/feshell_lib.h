#ifndef _feshell_lib
#define _feshell_lib

#define MAX_DIM_BUFF 1024

void shellInfo();
int countTokens(char *, char *);
int cd(char **);
int execute(int, char **);

#endif
