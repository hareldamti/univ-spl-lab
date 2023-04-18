#include <linux/limits.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include "LineParser.h"
#include <sys/wait.h>
#include <sys/types.h>

static bool debugMode = false;

void execute(cmdLine *pCmdLine) {
    if (memcmp(pCmdLine->arguments[0], "cd", 2) == 0) {
        if (chdir(pCmdLine->arguments[1]))
            fprintf(stderr, "Error: No such directory (%s)", pCmdLine->arguments[1]);
        return;
    }

    if (memcmp(pCmdLine->arguments[0], "kill", 4) == 0) {
        kill(atoi(pCmdLine->arguments[1]), SIGKILL);
        return;
    }

    if (memcmp(pCmdLine->arguments[0], "wake", 4) == 0) {
        kill(atoi(pCmdLine->arguments[1]), SIGCONT);
        return;
    }

    if (memcmp(pCmdLine->arguments[0], "suspend", 7) == 0) {
        kill(atoi(pCmdLine->arguments[1]), SIGTSTP);
        return;
    }

    int pid = fork();
    if (debugMode && pid) {
        fprintf(stderr, "Started process\npid:\t%d\nexecuting command:\t", pid);
        for (int i=0; i < MAX_ARGUMENTS && pCmdLine->arguments[i] && pCmdLine->arguments[i][0] != '\0'; i++)
            fprintf(stderr, "%s ", pCmdLine->arguments[i]);
        fprintf(stderr, "\n");
    }
    
    if (!pid) {
        int r = -1, w = -1;
        if (pCmdLine->inputRedirect) {
            close(0);
            r = open(pCmdLine->inputRedirect, O_RDONLY);
            dup(r);
        }
        if (pCmdLine->outputRedirect) {
            close(1);
            w = open(pCmdLine->outputRedirect, O_CREAT | O_WRONLY, 0777);
            dup(w);
        }
        execvp(pCmdLine->arguments[0], pCmdLine->arguments);
        if (r != -1) close(r);
        if (w != -1) close(w);
        _exit(1);
    }

    if (pCmdLine->blocking) {
        int wstatus;
        if ( waitpid(pid, &wstatus, 0) == -1 ) {
            perror("waitpid() failed");
            exit(1);
        }
        if (WEXITSTATUS(wstatus) != 0){
            perror(NULL);
            exit(1);
        }
    }
}

int main(int argc, char** argv) {
    for (int i = 1; i < argc; i++)
        debugMode |= memcmp(argv[i], "-d", 2) == 0;
    
    char path[PATH_MAX];
    char input[1 << 11];
    cmdLine* pCmdLine = NULL;
    while(1) {
        getcwd(path, PATH_MAX);
        fprintf(stdout, "%s> ",path);
        fgets(input, 1 << 11, stdin);
        if (memcmp(input, "quit", 4) == 0) break;
        if (pCmdLine != NULL) freeCmdLines(pCmdLine);
        pCmdLine = parseCmdLines(input);
        execute(pCmdLine);
    }
    freeCmdLines(pCmdLine);
    exit(0);
}