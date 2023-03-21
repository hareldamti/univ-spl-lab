#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

char* map(char *array, int array_length, char (*f) (char)){
  char* mapped_array = (char*)(malloc(array_length * sizeof(char)));
  for (int i = 0; i < array_length; i++) 
    mapped_array[i] = (*f)(array[i]);
  return mapped_array;
}

/* Ignores c, reads and returns a character from stdin using fgetc. */
char my_get(char c){
  return fgetc(stdin);
}

/* If c is a number between 0x20 and 0x7E, cprt prints the character
   of ASCII value c followedby a new line. Otherwise, cprt prints the
   dot ('.') character. After printing, cprt returns the value of c unchanged. */
char cprt(char c){
  if (0x20 <= c && c <= 0x7E) printf("%c\n", c);
  else printf(".");
  return c;
}

/* Gets a char c and returns its encrypted form by adding 2 to its value.
   If c is not between 0x20 and 0x7E it is returned unchanged */
char encrypt(char c) {
  if (0x20 <= c && c <= 0x7E) return c + 2;
  return c;
}

/* Gets a char c and returns its decrypted form by reducing 2 from its value.
   If c is not between 0x20 and 0x7E it is returned unchanged */
char decrypt(char c) {
  if (0x20 <= c && c <= 0x7E) return c - 2;
  return c;
}

/* xprt prints the value of c in a hexadecimal representation
followed by a new line,and returns c unchanged. */
char xprt(char c) {
  printf("%x\n", c);
  return c;
}

int MAX_INPUT_LENGTH = 20, CARRAY_SIZE = 5;

typedef struct fun_desc {
    char *name;
    char (*fun)(char);
} fun_desc;

fun_desc menu[] = {
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
            fprintf(stdout, "%d) %s\n", i + 1, menu[i].name);
        fprintf(stdout, "Choice: ");
        if (fgets(input, MAX_INPUT_LENGTH, stdin) == NULL) break;
        int choice = atoi(input);
        if (choice < 1 || choice > menu_size) {
            fprintf(stdout, "Not within bounds\n");
            break;
        }
        fprintf(stdout, "Within bounds\n");
        char* eval = map(carray, CARRAY_SIZE, menu[choice - 1].fun);
        free(carray);
        carray = eval;
        fprintf(stdout, "Current string:\t%s\n", carray);
    }
    while (1);
    free(carray);
    exit(0);
}