#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include "./feshell_lib.h"
#include "./parse_lib/parse_lib.h"

int main(int argc, char *argv[]) {
    char buff[MAX_DIM_BUFF];

    shellInfo();
    while (fgets(buff, MAX_DIM_BUFF + 1, stdin) != NULL) {
        if (!strcmp(buff, "\n") || !strlen(buff)) {
            shellInfo();
            continue;
        }
        else if (!strcmp(buff, "clear")) {
            system("clear");
            continue;
        }

        parse(buff);

        shellInfo();
        fflush(stdin);
    }

    if (feof(stdin)) {
        printf("exit\n");
        kill(getppid(), SIGINT);
    }

    return 0;
}
