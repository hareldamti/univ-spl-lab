#include <linux/limits.h>
#include <unistd.h>
#include <stdio.h>
#include "LineParser.h"

int main(int argc, char** argv) {
    char path[PATH_MAX];
    char input[1 << 11];
    getcwd(path, PATH_MAX);
    while(1) {
        fprintf(stdout, "%s> ",path);
        fgets(input, 1 << 11, stdin);
        //fgetc(stdin);
        fprintf(stdout, "\n%s\n",input);

    }
}