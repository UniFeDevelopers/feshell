#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

void shellInfo() {
    char hostn[1024] = "";
    char currentDirectory[1024] = "";
    gethostname(hostn, sizeof(hostn));
    hostn[strlen(hostn) - 6] = '\0';
    printf("\x1b[1m\x1B[32m%s@%s\x1b[0m:\x1b[1m\x1B[34m%s \x1b[0m$ ", getenv("LOGNAME"), hostn, getcwd(currentDirectory, 1024));
}

int cd(char *args[]) {
    if (args[1] == NULL) {
        chdir(getenv("HOME"));
        return 0;
    }
    else{
        if (chdir(args[1]) == -1) {
            fprintf(stderr, "-feshell: %s: %s: ", args[0], args[1]);
            perror("");
            return -1;
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    int pid, status;
    char buff[1024];
    char *cmd;
    char **args = NULL;
    int n_args;

    shellInfo();
    while (fgets(buff, 1025, stdin) != NULL) {
        args = NULL;
        n_args = 0;

        if (!strcmp(buff, "\n")) {
            shellInfo();
            continue;
        }
        else if (!strcmp(buff, "clear")) {
            system("clear");
            continue;
        }

        cmd = strtok(buff, " \t\n;");

        if (strstr(cmd, "exit") != NULL) {
            exit(0);
        }

        while (cmd != NULL) {
            args = realloc(args, sizeof (char*) * ++n_args);

            if (args == NULL) exit(1);

            if (strlen(cmd)) {
                args[n_args - 1] = cmd;
            }

            cmd = strtok(NULL, " \t\n;");
        }

        if (n_args) {
            if (strstr(args[0], "cd") != NULL) {
                cd(args);
                shellInfo();
                continue;
            }

            pid = fork();

            if (pid == 0) {
                if (execvp(args[0], args) == -1) {
                    fprintf(stderr, "-feshell: %s: command not found\n", args[0]);
                }
            } else if (pid > 0) {
                pid = wait(&status);
                /*
                 * gestione dello stato
                 */
            } else {
                fprintf(stderr, "-feshell: fork fallita");
            }
        }

        shellInfo();
        fflush(stdin);
    }

    if (feof(stdin)) {
        printf("exit\n");
        kill(getppid(), SIGINT);
    }

    free(args);

    return 0;
}
