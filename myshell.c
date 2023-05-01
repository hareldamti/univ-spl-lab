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
#define HISTLEN 5

typedef struct process{
    cmdLine* cmd;                     /* the parsed command line*/
    pid_t pid; 		                  /* the process id that is running the command*/
    int status;                       /* status of the process: RUNNING/SUSPENDED/TERMINATED */
    struct process *next;	          /* next process in chain */
} process;

typedef struct historyQueue{
    char* commands[HISTLEN];
    int idx;
    int full;
} historyQueue;
historyQueue history;

void execute(process** processList, cmdLine *pCmdLine, int doFork);

void addHistoryLine(char* line) {
    if (line[0] == '!') return;
    if (history.commands[history.idx]) free(history.commands[history.idx]);
    history.commands[ history.idx ] = malloc(strlen(line));
    strcpy(history.commands[ history.idx ], line);
    history.idx++;
    if (history.idx == HISTLEN) {history.idx = 0; history.full = 1;}
}

void freeHistory() {
    for (int i = 0; i < HISTLEN; i++) 
        if (history.commands[i]) free(history.commands[i]);
        else break;
}

char* fetchHistoryLine(int n) {
    if (!history.full && n > history.idx){
        fprintf(stdout, "No such entry yet\n");
        return NULL;
    }
    if (n > HISTLEN){
        fprintf(stdout, "Out of range\n");
        return NULL;
    }
    return history.commands[(history.idx - n) % HISTLEN];
}

void printHistory() {

    if (!history.full) for (int i = 0; i < history.idx; i++) {
            fprintf(stdout, "[%d] %s", i, history.commands[i]);
    }
    else for (int i = 0; i < HISTLEN; i++) {
            fprintf(stdout, "[%d] %s", i, history.commands[(history.idx + i) % HISTLEN]);
    }
}

void addProcess(process** process_list, cmdLine* cmd, pid_t pid) {
    process* newProcess = malloc(sizeof(process));
    newProcess->cmd = cmd;
    newProcess->pid = pid;
    if (process_list) newProcess->next = *process_list;
    *process_list = newProcess;
}

void freeProcessList(process* process_list) {
    if (!process_list) return;
    if (process_list->next) freeProcessList(process_list->next);
    if (process_list->cmd) freeCmdLines(process_list->cmd);
    free(process_list);
}

void updateProcessList(process **process_list) {
    for (process* curr = *process_list; curr; curr = curr->next) {
        int wstatus;
        if (waitpid(curr->pid, &wstatus, WNOHANG | WUNTRACED | WCONTINUED) == -1) curr->status = TERMINATED;
        else if (WIFSTOPPED(wstatus) || WIFSIGNALED(wstatus)) {curr->status = SUSPENDED;}
        else curr->status = RUNNING;
    }
}

void updateProcessStatus(process **process_list, int pid, int status) {
    if (!process_list) return;
    for (process* curr = *process_list; curr; curr = curr->next) 
        if (curr->pid == pid) {
            curr->status = status;
            return;
        }
}

void printProcessList(process** process_list) {
    if (!process_list) return;
    updateProcessList(process_list);
    fprintf(stdout, "PID\tCommand\tSTATUS\n");
    
    process* curr = *process_list, *prev;
    while (curr) {
        fprintf(stdout, "%d\t%s\t%s\n",
        curr->pid,
        curr->cmd->arguments[0],
        curr->status==1 ? "RUNNING" : (curr->status == 0 ? "SUSPENDED" : "TERMINATED"));
        if (curr->status == -1) {
            // Delete after displaying TERMINATED
            if (!prev)
                *process_list = curr->next;
            else prev->next = curr->next;
        }
        else prev = curr;
        process* next = curr->next;
        if (curr->status == -1) {
            curr->next = NULL;
            freeProcessList(curr);
        }
        curr = next;
    }
}

void executeSelf(process** processList, cmdLine *pCmdLine, int doFork) {
    if (memcmp(pCmdLine->arguments[0], "cd", 2) == 0) {
        if (chdir(pCmdLine->arguments[1]))
            fprintf(stderr, "Error: No such directory (%s)", pCmdLine->arguments[1]);
        freeCmdLines(pCmdLine);
        return;
    }

    if (memcmp(pCmdLine->arguments[0], "procs", 5) == 0) {
        printProcessList(processList);
        freeCmdLines(pCmdLine);
        return;
    }

    if (memcmp(pCmdLine->arguments[0], "kill", 4) == 0) {
        kill(atoi(pCmdLine->arguments[1]), SIGKILL);
        freeCmdLines(pCmdLine);
        return;
    }

    if (memcmp(pCmdLine->arguments[0], "wake", 4) == 0) {
        kill(atoi(pCmdLine->arguments[1]), SIGCONT);
        freeCmdLines(pCmdLine);
        return;
    }

    if (memcmp(pCmdLine->arguments[0], "suspend", 7) == 0) {
        kill(atoi(pCmdLine->arguments[1]), SIGTSTP);
        freeCmdLines(pCmdLine);
        return;
    }

    if (memcmp(pCmdLine->arguments[0], "history", 7) == 0) {
        printHistory();
        freeCmdLines(pCmdLine);
        return;
    }

    if (memcmp(pCmdLine->arguments[0], "!!", 2) == 0) {
        cmdLine* newCmdLine;
        char* line = fetchHistoryLine(1);
        if (line) {
            newCmdLine = parseCmdLines(line);
            execute(processList, newCmdLine, 1);
        }
        freeCmdLines(pCmdLine);
        return;
    }

    if (memcmp(pCmdLine->arguments[0], "!", 1) == 0) {
        int n = atoi(pCmdLine->arguments[0] + 1);
        cmdLine* newCmdLine;
        char* line = fetchHistoryLine(n);
        if (line) {
            newCmdLine = parseCmdLines(line);
            execute(processList, newCmdLine, 1);
        }
        freeCmdLines(pCmdLine);
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
        addProcess(processList, pCmdLine, pid);
        if (pCmdLine->blocking) {
            int wstatus;
            if ( waitpid(pid, &wstatus, 0) == -1 ) {
                perror("waitpid() failed");
                exit(1);
            }
            if (WEXITSTATUS(wstatus) != 0){
                perror("child process failed");
                exit(1);
            }
        }
    }
}

void execute(process** processList, cmdLine *pCmdLine, int doFork) {
    if (!pCmdLine->next) {
        executeSelf(processList, pCmdLine, doFork);
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
        executeSelf(processList, pCmdLine, 0);
        close(pipefd[1]);
    }
    close(pipefd[1]);
    int pid2 = fork();
    if (!pid2) {
        close(0);
        dup(pipefd[0]);
        execute(processList, pCmdLine->next, 0);
        exit(0);
    }
    close(pipefd[0]);
    addProcess(processList, pCmdLine, pid1);
    addProcess(processList, pCmdLine->next, pid2);
    pCmdLine->next = NULL;
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
}

int main(int argc, char** argv) {
    for (int i = 1; i < argc; i++)
        debugMode |= memcmp(argv[i], "-d", 2) == 0;
    
    char path[PATH_MAX];
    char input[1 << 11];

    history.full = 0;
    history.idx = 0;

    process* pProcess = NULL;
    process** processList = &pProcess;

    while(1) {
        cmdLine* pCmdLine = NULL;
        getcwd(path, PATH_MAX);
        fprintf(stdout, "%s> ",path);
        fgets(input, 1 << 11, stdin);
        if (memcmp(input, "quit", 4) == 0) { break; }
        addHistoryLine(input);
        pCmdLine = parseCmdLines(input);
        execute(processList, pCmdLine, 1);
    }
    freeProcessList(*processList);
    freeHistory();
    exit(0);
}