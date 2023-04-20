#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

int main(int argc, char ** argv) {
    
    // Create a pipe
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(1);
    }
    char* args1[3] = {"ls","-l", 0};

    fprintf(stderr, "(parent_process>forking...)\n");
    int pid1 = fork();
    
    if (!pid1) {
        //child1 process
        fprintf(stderr, "(child1>redirecting stdout to the write end of the pipe...)\n");
        close(1);
        dup(pipefd[1]);
        close(pipefd[0]);
        fprintf(stderr, "(child1>going to execute cmd: ls -l)\n");
        execvp(args1[0],args1);
        close(pipefd[1]);
        _exit(1);
    }

    //parent process
    fprintf(stderr, "(parent_process>created process with id: %d)\n", pid1);
    fprintf(stderr, "(parent_process>closing the write end of the pipe...)\n");
    close(pipefd[1]);

    char* args2[4] = {"tail","-n", "2", 0};

    fprintf(stderr, "(parent_process>forking...)\n");
    int pid2 = fork();
    if (!pid2) {
        //child2 process
        fprintf(stderr, "(child2>redirecting stdin to the read end of the pipe...)\n");
        close(0);
        dup(pipefd[0]);
        fprintf(stderr, "(child2>going to execute cmd: tail -n 2)\n");
        execvp(args2[0], args2);
        close(pipefd[0]);
        _exit(1);
    }
    
    fprintf(stderr, "(parent_process>created process with id: %d)\n", pid2);
    fprintf(stderr, "(parent_process>closing the read end of the pipe...)\n");
    close(pipefd[0]);
    fprintf(stderr, "(parent_process>waiting for child processes to terminate…)\n");
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
    fprintf(stderr, "(parent_process>exiting)\n");
}