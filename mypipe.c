#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

int main(int argc, char ** argv) {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(1);
    }
    
    int pid = fork();
    if (!pid) {
        //child process
        close(1);
        close(pipefd[0]);
        dup(pipefd[1]);
        write(1, "hello\n", 7);
        exit(0);
    }
    //parent process
    close(0);
    dup(pipefd[0]);
    close(pipefd[1]);
   
    char input[20];
    read(0, input, 20);
    waitpid(pid, NULL, 0);
    fprintf(stdout, "Received message from child: %s", input);
    exit(0);
}