#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "base_functions.c"

int main(int argc, char **argv){
  if (argc > 1) {
    int l = 0;
    while (argv[1][l] != '\0') l++;

    printf("my_get: \n");
    char* mapped = map(argv[1], l, &my_get);
    printf("Returned string: %s\n\n", mapped); // Without null termination.. how?


    printf("cprt: \n");
    mapped = map(mapped, l, &cprt);
    printf("Returned string: %s\n\n", mapped); // Without null termination.. how?


    printf("encrypt: \n");
    mapped = map(mapped, l, &encrypt);
    printf("Returned string: %s\n\n", mapped); // Without null termination.. how?

    printf("decrypt: \n");
    mapped = map(mapped, l, &decrypt);
    printf("Returned string: %s\n\n", mapped); // Without null termination.. how?


    printf("xprt: \n");
    mapped = map(mapped, l, &xprt);
    printf("Returned string: %s\n\n", mapped); // Without null termination.. how?
    free(mapped);
  }
  
}

