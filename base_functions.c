#include <stdio.h>
#include <malloc.h>

char* map(char *array, int array_length, char (*f) (char)){
  char* mapped_array = (char*)(malloc(array_length*sizeof(char)));
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