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
    free(mapped);

    printf("cprt: \n");
    mapped = map(argv[1], l, &cprt);
    printf("Returned string: %s\n\n", mapped); // Without null termination.. how?
    free(mapped);

    printf("encrypt: \n");
    mapped = map(argv[1], l, &encrypt);
    printf("Returned string: %s\n\n", mapped); // Without null termination.. how?
    free(mapped);

    printf("decrypt: \n");
    mapped = map(argv[1], l, &decrypt);
    printf("Returned string: %s\n\n", mapped); // Without null termination.. how?
    free(mapped);

    printf("xprt: \n");
    mapped = map(argv[1], l, &xprt);
    printf("Returned string: %s\n\n", mapped); // Without null termination.. how?
    free(mapped);
  }
  
}

