#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>


int main(int argc, char ** argv) {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(1);
    }
    close(0);
    dup(pipefd[0]);
    int pid = fork();
    if (!pid) {
        //child process
        close(1);
        dup(pipefd[1]);
        write(1, "hello\n", 7);
        exit(0);
    }
    //parent process
    char input[20];
    read(0, input, 20);
    fprintf(stdout, "Received message from child: %s", input);
    exit(0);
}