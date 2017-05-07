typedef struct _parsedInput {
    int flag_a;
    int flag_l;
    int flag_t;
    int flag_h;
    int no_color;
    char* path;
} parsedInput;

void list_dir(int, char **);
char *strrep(char *, char *, char *);
