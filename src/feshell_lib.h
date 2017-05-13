#ifndef _feshell_lib
#define _feshell_lib

#define MAX_DIM_BUFF 1024

void shellInfo();
int countTokens(char *, char *);
int cd(char **);
void execute(int, char **);

#endif
