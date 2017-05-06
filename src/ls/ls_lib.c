#include "./ls_lib.h"
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

parsedInput* parse_input(int n_args, char **args) {
    parsedInput *input;
    input = (parsedInput*) malloc(sizeof(parsedInput));

    input->flag_a = input->flag_l = input->flag_t = input->flag_h = 0;
    input->path = NULL;

    int i;
    for (i = 1; i < n_args; i++) {
        if (strstr(args[i], "-") != NULL) {
            if (strstr(args[i], "a") != NULL) {
                input->flag_a = 1;
            }
            if (strstr(args[i], "l") != NULL) {
                input->flag_l = 1;
            }
            if (strstr(args[i], "t") != NULL) {
                input->flag_t = 1;
            }
            if (strstr(args[i], "h") != NULL) {
                input->flag_h = 1;
            }
        }
        else {
            if (input->path != NULL) {
                return NULL;
            }

            input->path = (char *) malloc(sizeof(char) * strlen(args[i]));
            strcpy(input->path, args[i]);
        }
    }

    return input;
}

char *strrep(char *str, char *orig, char *rep) {
    static char buffer[4096];
    char *p;

    if (!(p = strstr(str, orig))) return str;

    strncpy(buffer, str, p-str);
    buffer[p-str] = '\0';

    sprintf(buffer + (p - str), "%s%s", rep, p + strlen(orig));

    return buffer;
}

void list_dir(int n_args, char **args) {
    parsedInput *input;
    struct dirent *ent;
    DIR *dp = NULL;

    input = parse_input(n_args, args);

    char currentDirectory[1024] = "";
    char *path = input->path != NULL ? input->path : getcwd(currentDirectory, 1024);

    dp = opendir((const char*) strrep(path, "~", getenv("HOME")));

    if (dp == NULL) {
        fprintf(stderr, "-feshell: ls: %s: ", path);
        perror("");
    }

    struct stat fileStat;
    struct passwd *pwd;
    struct group *grp;
    time_t st_time;
    char time[100];
    char *path_tmp;

    ent = readdir(dp);

    while (ent != NULL) {
        if ((!input->flag_a && ent->d_name[0] != '.') || input->flag_a) {
            if (!input->flag_l) {
                printf("%s\t", ent->d_name);
            }
            else {
                path_tmp = (char *) malloc(sizeof(char) * (strlen(strrep(path, "~", getenv("HOME"))) + strlen(ent->d_name) + 1));

                strcpy(path_tmp, strrep(path, "~", getenv("HOME")));
                strcat(path_tmp, "/");
                strcat(path_tmp, ent->d_name);

                if (stat(path_tmp, &fileStat) < 0) {
                    fprintf(stderr, "-feshell: ls: %s: ", path_tmp);
                    perror("");
                }
                else {
                    pwd = getpwuid(fileStat.st_uid);
                    grp = getgrgid(fileStat.st_gid);
                    st_time = fileStat.st_mtime;
                    strftime(time, 100, "%d %b %H:%M", localtime(&st_time));

                    printf((S_ISDIR(fileStat.st_mode)) ? "d" : "-");
                    printf((fileStat.st_mode & S_IRUSR) ? "r" : "-");
                    printf((fileStat.st_mode & S_IWUSR) ? "w" : "-");
                    printf((fileStat.st_mode & S_IXUSR) ? "x" : "-");
                    printf((fileStat.st_mode & S_IRGRP) ? "r" : "-");
                    printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
                    printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");
                    printf((fileStat.st_mode & S_IROTH) ? "r" : "-");
                    printf((fileStat.st_mode & S_IWOTH) ? "w" : "-");
                    printf((fileStat.st_mode & S_IXOTH) ? "x" : "-");
                    printf("  %3hu", (unsigned short) fileStat.st_nlink);
                    printf("  %s", pwd->pw_name);
                    printf("  %s", grp->gr_name);
                    printf("  %6lld", (long long int) fileStat.st_size);
                    printf("  %s", time);
                    printf("  %s", ent->d_name);
                }

                printf("\n");
                free(path_tmp);
            }
        }

        ent = readdir(dp);
    }

    if (!input->flag_l) printf("\n");

    free(pwd);
    free(grp);
    free(path);
    free(input);
    free(ent);
}
