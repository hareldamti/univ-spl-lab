#include <stdio.h>
#include <stdlib.h>

#include "base_functions.c"

int MAX_INPUT_LENGTH = 20,
    CARRAY_SIZE = 5;

struct fun_desc {
    char *name;
    char (*fun)(char);
};

struct fun_desc menu[] = {
    { "my_get", &my_get },
    { "cprt", &cprt },
    { "encrypt", &encrypt },
    { "decrypt", &decrypt },
    { "xprt", &xprt },
    { NULL, NULL }
};

int main(int argc, char **argv) {
    
    char input[MAX_INPUT_LENGTH];
    
    char* carray = malloc(CARRAY_SIZE * sizeof(char));
    carray[0] = '\0';
    int menu_size = 0;
    while (menu[menu_size].name != NULL) menu_size++;

    do {
        fprintf(stdout, "Please choose a function (ctrl^D for exit):\n");
        for (int i = 0; i < menu_size; i++)
            fprintf(stdout, "%d) %s\n", i, menu[i].name);
        fprintf(stdout, "\t");
        
        fflush(stdin);
        fgets(input, MAX_INPUT_LENGTH, stdin);
        if (feof(stdin)) break;
        printf("%s\n", input);
        int choice = atoi(input);
        if (choice < 0 || choice >= menu_size) {
            fprintf(stdout, "Not within bounds\n");
            break;
        }
        fprintf(stdout, "Within bounds\n");
        char* eval = map(carray, CARRAY_SIZE, menu[choice].fun);
        free(carray);
        carray = eval;
        fprintf(stdout, "Current string:\t%s\n", carray);
    }
    while (!feof(stdin));
}