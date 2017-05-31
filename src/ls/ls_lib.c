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
    printf("Licensed with GNU General Public Licence v3\n\n");
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

    struct stat fileStat;
    struct passwd *pwd;
    struct group *grp;
    time_t st_time;
    char time[100];
    char path_tmp[1024];
    char *actualpath;
    double size;
    char um[2];
    char buffer[1024];
    int i, countDir = 0;

    char currentDirectory[1024] = "";

    dir_entry *entries;
    dir_entry ent_tmp;
    strcpy(ent_tmp.data, "");
    strcpy(ent_tmp.name, "");

    int num_ents = 0;

    input = parse_input(n_args, args);

    if (input == NULL) {
        return; // ha già stampato usage()
    }

    char *path = input->path != NULL ? input->path : getcwd(currentDirectory, 1024);

    dp = opendir((const char*) strrep(path, "~", getenv("HOME")));
    if (dp != NULL) {
        countDir = 0;
        ent = readdir(dp);
        while (ent != NULL ) {
            if ((!input->flag_a && ent->d_name[0] != '.') || input->flag_a) {
                countDir++;
            }

            ent = readdir(dp);
        }
        closedir(dp);
    }
    else {
        fprintf(stderr, "-feshell: ls: %s: ", path);
        perror("");
        return;
    }

    entries = (dir_entry *) malloc(sizeof(dir_entry) * countDir);

    dp = opendir((const char*) strrep(path, "~", getenv("HOME")));
    if (dp == NULL) {
        fprintf(stderr, "-feshell: ls: %s: ", path);
        perror("");
        return;
    }

    ent = readdir(dp);

    while (ent != NULL) {
        if ((!input->flag_a && ent->d_name[0] != '.') || input->flag_a) {
            strcpy(ent_tmp.data, "");
            strcpy(ent_tmp.name, "");

            strcpy(path_tmp, strrep(path, "~", getenv("HOME")));
            strcat(path_tmp, "/");
            strcat(path_tmp, ent->d_name);

            if (lstat(path_tmp, &fileStat) < 0) {
                fprintf(stderr, "-feshell: ls: %s: ", path_tmp);
                perror("");
                exit(1);
            }

            st_time = fileStat.st_mtime;
            ent_tmp.time = st_time;

            if (!input->flag_l) {
                strcat(ent_tmp.data, (S_ISDIR(fileStat.st_mode)) ? "\x1B[34m" : "\x1B[0m");
                sprintf(buffer, "%s\t", ent->d_name);
                strcat(ent_tmp.data, buffer);
            }
            else {
                pwd = getpwuid(fileStat.st_uid);
                grp = getgrgid(fileStat.st_gid);
                strftime(time, 100, "%d %b %H:%M", localtime(&st_time));

                strcat(ent_tmp.data, (S_ISDIR(fileStat.st_mode)) ? "d" : "-");
                strcat(ent_tmp.data, (fileStat.st_mode & S_IRUSR) ? "r" : "-");
                strcat(ent_tmp.data, (fileStat.st_mode & S_IWUSR) ? "w" : "-");
                strcat(ent_tmp.data, (fileStat.st_mode & S_IXUSR) ? "x" : "-");
                strcat(ent_tmp.data, (fileStat.st_mode & S_IRGRP) ? "r" : "-");
                strcat(ent_tmp.data, (fileStat.st_mode & S_IWGRP) ? "w" : "-");
                strcat(ent_tmp.data, (fileStat.st_mode & S_IXGRP) ? "x" : "-");
                strcat(ent_tmp.data, (fileStat.st_mode & S_IROTH) ? "r" : "-");
                strcat(ent_tmp.data, (fileStat.st_mode & S_IWOTH) ? "w" : "-");
                strcat(ent_tmp.data, (fileStat.st_mode & S_IXOTH) ? "x" : "-");
                sprintf(buffer, " %4hu", (unsigned short) fileStat.st_nlink);
                strcat(ent_tmp.data, buffer);
                sprintf(buffer, " %8s", pwd->pw_name);
                strcat(ent_tmp.data, buffer);
                sprintf(buffer, " %8s", grp->gr_name);
                strcat(ent_tmp.data, buffer);

                if (!input->flag_h) {
                    sprintf(buffer, " %7lld", (long long int) fileStat.st_size);
                    strcat(ent_tmp.data, buffer);
                }
                else {
                    if (fileStat.st_size) {
                        size = log((double) fileStat.st_size) / log(1024);
                        size = (fileStat.st_size - (long int) ceil(size) < (long int) floor(size) - fileStat.st_size ? ceil(size) : floor(size));
                    }
                    else {
                        size = (double) fileStat.st_size;
                    }

                    strcpy(um, MEASURE_UNITS[(int) size]);
                    size = (double) fileStat.st_size / pow(1024, size);
                    sprintf(buffer, " %7.3g%s", size, um);
                    strcat(ent_tmp.data, buffer);
                }

                sprintf(buffer, " %s", time);
                strcat(ent_tmp.data, buffer);

                if (!input->no_color) {
                    if (S_ISDIR(fileStat.st_mode)) {
                        strcat(ent_tmp.data, "\x1B[34m");
                    }
                    else if (fileStat.st_mode & S_IXUSR) {
                        strcat(ent_tmp.data, "\x1B[31m");
                    }
                    if (S_ISLNK(fileStat.st_mode)) {
                        strcat(ent_tmp.data, "\x1B[35m");
                    }
                }

                sprintf(buffer, " %s\x1B[0m", ent->d_name);
                strcat(ent_tmp.data, buffer);
                strcpy(ent_tmp.name, ent->d_name);

                if (S_ISDIR(fileStat.st_mode)) {
                    strcat(ent_tmp.data, "/");
                }
                else if (S_ISLNK(fileStat.st_mode)) {
                    strcat(ent_tmp.data, "@ -> ");
                    actualpath = realpath(path_tmp, NULL);
                    if (actualpath != NULL) {
                        strcat(ent_tmp.data, actualpath);
                    }
                }
                else if (fileStat.st_mode & S_IXUSR) {
                    strcat(ent_tmp.data, "*");
                }

                strcat(ent_tmp.data, "\n");
            }

            strcpy(entries[num_ents].data, ent_tmp.data);
            strcpy(entries[num_ents].name, ent_tmp.name);
            entries[num_ents].time = ent_tmp.time;
            num_ents++;
        }

        ent = readdir(dp);
    }

    if (input->flag_t) {
        sort(entries, countDir, "time");
    }
    else {
        sort(entries, countDir, "alpha");
    }

    for (i = 0; i <= countDir; i++) {
        printf("%s", entries[i].data);
    }

    if (!input->flag_l) {
        printf("\n");
    }
}
