#include <time.h>

#ifndef _ls_lib
#define _ls_lib
#define MAX_NUM_DIR 1024

typedef struct _parsedInput {
    int flag_a;
    int flag_l;
    int flag_t;
    int flag_h;
    int no_color;
    char* path;
} parsedInput;

typedef struct _dir_entry {
    char data[MAX_NUM_DIR];
    char name[1024];
    time_t time;
} dir_entry;

void list_dir(int, char **);
char *strrep(char *, char *, char *);

#endif
