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

#define TERMINATED  -1
#define RUNNING 1
#define SUSPENDED 0
    
typedef struct process{
    cmdLine* cmd;                     /* the parsed command line*/
    pid_t pid; 		                  /* the process id that is running the command*/
    int status;                       /* status of the process: RUNNING/SUSPENDED/TERMINATED */
    struct process *next;	          /* next process in chain */
} process;

process** processList;

void addProcess(process** process_list, cmdLine* cmd, pid_t pid) {
    process* newProcess = malloc(sizeof(process));
    newProcess->cmd = cmd;
    newProcess->next = *process_list;
    newProcess->pid = pid;
}

void freeProcessList(process* process_list) {
    if (!process_list) return;
    if (process_list->next) freeProcessList(process_list->next);
    freeCmdLines(process_list->cmd);
    free(process_list);
}

void updateProcessList(process **process_list) {
    process* curr = *process_list, *prev;
    while (curr) {
        int wstatus;

        if (waitpid(curr->pid, &wstatus, WNOHANG) == -1 || WIFEXITED(wstatus)) {
            if (!prev) 
                process_list = &curr->next;
            else prev->next = curr->next;
        };
        prev = curr;
        curr = curr->next;
    }
}

void updateProcessList(process **process_list) {
    for (process* curr = *process_list; curr; curr = curr->next) {
        int wstatus;
        waitpid(curr->pid, &wstatus, WNOHANG);
        if (WIFCONTINUED(wstatus)) curr->status = RUNNING;
        if (WIFSTOPPED(wstatus)) curr->status = SUSPENDED;
        if (WIFEXITED(wstatus)) curr->status = TERMINATED;
    }
}



void printProcessList(process** process_list) {
    fprintf(stdout, "PID\tCommand\tSTATUS\n");
    for (process* curr = *process_list; curr; curr = curr->next)
        fprintf(stdout, "%d\t%s\t%s\n",
        curr->pid,
        curr->cmd->arguments[0],
        curr->status==1?"RUNNING":(curr->status==0?"SUSPENDED":"TERMINATED"));
}

void executeSelf(cmdLine *pCmdLine, int doFork) {
    if (memcmp(pCmdLine->arguments[0], "cd", 2) == 0) {
        if (chdir(pCmdLine->arguments[1]))
            fprintf(stderr, "Error: No such directory (%s)", pCmdLine->arguments[1]);
        return;
    }

    if (memcmp(pCmdLine->arguments[0], "procs", 5) == 0) {
        printProcessList(processList);
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

    int pid = 0;
    if (doFork) pid = fork();
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

    if (doFork) {
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
}

void execute(cmdLine *pCmdLine, int doFork) {

    if (!pCmdLine->next) {
        executeSelf(pCmdLine, doFork);
        return;
    }

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(1);
    }
    int pid1 = fork();
    if (!pid1) {
        close(1);
        dup(pipefd[1]);
        close(pipefd[0]);
        executeSelf(pCmdLine, 0);
        close(pipefd[1]);
    }
    close(pipefd[1]);
    int pid2 = fork();
    if (!pid2) {
        close(0);
        dup(pipefd[0]);
        execute(pCmdLine->next, 0);
        exit(0);
    }
    close(pipefd[0]);
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
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
        execute(pCmdLine, 1);
    }
    freeCmdLines(pCmdLine);
    exit(0);
}