#include <linux/limits.h>
#include <unistd.h>
#include "LineParser.h"

int main(int argc, char** argv) {
    char path[PATH_MAX];
    char input[1 << 11];
    getcwd(path, PATH_MAX);
    while(1) {
        fprint("> %s",path);
        fgets(input);
        fgetc();
        fprint("\n%s\n",input);

    }
}