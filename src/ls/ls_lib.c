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
#include <math.h>

char *MEASURE_UNITS[] = {"B", "K", "M", "G", "T"};

void usage() {
    printf("Usage: ls [OPTION]... [FILE]...\n");
    printf("List information about the FILEs (the current directory by default).\n");
    printf("Sort entries alphabetically if -t is not specified.\n\n");

    printf("The following options are available:\n");
    printf("  -a\tdo not ignore entries starting with .\n");
    printf("  -l\tuse a long listing format\n");
    printf("  -h\twith -l, print human readable sizes (e.g., 1K 234M 2G)\n");
    printf("  -t\tsort by modification time, newest first\n\n");

    printf("Using color to distinguish file types is enabled by default.\n");
    printf("With --no-color, you can disable this behavior\n\n");

    printf("Documentation and code repository at: <https://github.com/feshellWithLove/feshell>\n");
    printf("Licensed with GNU Public Licence v3\n\n");
    printf("Authors:\nAntonio Guerra, Nicola Zambello\n");
}

parsedInput* parse_input(int n_args, char **args) {
    parsedInput *input;
    input = (parsedInput*) malloc(sizeof(parsedInput));

    input->flag_a = input->flag_l = input->flag_t = input->flag_h = input->no_color = 0;
    input->path = NULL;

    int i;
    for (i = 1; i < n_args; i++) {
        if (strstr(args[i], "--help") != NULL) {
            usage();
            return NULL;
        }
        if (strstr(args[i], "--no-color") != NULL) {
            input->no_color = 1;
            continue;
        }
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
                fprintf(stderr, "ls: illegal argument exception\nUsage: [-alth] [file ...]\n");
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

    if (input == NULL) {
        exit(EXIT_FAILURE);
    }

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
    char *actualpath;
    double size;
    char um[2];

    ent = readdir(dp);

    while (ent != NULL) {
        if ((!input->flag_a && ent->d_name[0] != '.') || input->flag_a) {
            if (!input->flag_l) {
                printf((S_ISDIR(fileStat.st_mode)) ? "\x1B[34m" : "\x1B[0m");
                printf("%s\t", ent->d_name);
            }
            else {
                path_tmp = (char *) malloc(sizeof(char) * (strlen(strrep(path, "~", getenv("HOME"))) + strlen(ent->d_name) + 1));

                strcpy(path_tmp, strrep(path, "~", getenv("HOME")));
                strcat(path_tmp, "/");
                strcat(path_tmp, ent->d_name);

                if (lstat(path_tmp, &fileStat) < 0) {
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
                    printf(" %4hu", (unsigned short) fileStat.st_nlink);
                    printf(" %8s", pwd->pw_name);
                    printf(" %8s", grp->gr_name);

                    if (!input->flag_h) {
                        printf(" %7lld", (long long int) fileStat.st_size);
                    }
                    else {
                        size = log((double) fileStat.st_size) / log(1024);
                        size = (fileStat.st_size - (long int) ceil(size) < (long int) floor(size) - fileStat.st_size ? ceil(size) : floor(size));
                        strcpy(um, MEASURE_UNITS[(int) size]);
                        size = (double) fileStat.st_size / pow(1024, size);
                        printf(" %7.3g%s", size, um);
                    }

                    printf(" %s", time);

                    if (!input->no_color) {
                        if (S_ISDIR(fileStat.st_mode)) printf("\x1B[34m");
                        else if (fileStat.st_mode & S_IXUSR) printf("\x1B[31m");
                        if (S_ISLNK(fileStat.st_mode)) printf("\x1B[35m");
                    }

                    printf(" %s\x1B[0m", ent->d_name);

                    if (S_ISDIR(fileStat.st_mode)) printf("/");
                    else if (S_ISLNK(fileStat.st_mode)) {
                        printf("@ -> ");
                        actualpath = realpath(path_tmp, NULL);
                        if (actualpath != NULL) {
                            printf("%s", actualpath);
                        }
                    }
                    else if (fileStat.st_mode & S_IXUSR) printf("*");
                }

                printf("\n");
                free(path_tmp);
            }
        }

        ent = readdir(dp);
    }

    if (!input->flag_l) printf("\n");

    /*
    free(pwd);
    free(grp);
    free(path);
    free(input);
    free(ent);
    free(actualpath);
    */
}
