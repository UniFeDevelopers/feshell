#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int pid, status;
    pid=fork();

    if(pid==0) {
        execl("/bin/ls", "ls","-l", (char*) 0);
        printf("exec fallita");
    } else if (pid>0){
        pid=wait(&status);
        //<gestione dello stato>
    } else {
        printf("fork fallita");
    }

    return 0;
}
