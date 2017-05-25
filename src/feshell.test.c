#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>

#include "./feshell_lib.h"
#include "./parse_lib/parse_lib.h"

int main(void) {
    char *exec_args[] = { "ls", "-lah", 0 };

    printf("\n\nESECUZIONE TEST:\tls -lah\n\n");
    execute(2, exec_args);

    return 0;
}
