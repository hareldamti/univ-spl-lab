#include "util.h"

#define SYS_WRITE 4
#define STDOUT 1
#define SYS_OPEN 5
#define O_RDWR 2
#define SYS_SEEK 19
#define SEEK_SET 0

extern int system_call();

int main (int argc , char* argv[], char* envp[])
{  
  int i, l;
  char seperator[1] = " ";
  for (i = 0; i < argc; i++) {
    for (l = 0; argv[i][l] != '\0'; l++) {}
      system_call(SYS_WRITE, STDOUT, argv[i], l);
    system_call(SYS_WRITE, STDOUT, seperator, 1);
  }
  seperator[0] = '\n';
  system_call(SYS_WRITE, STDOUT, seperator, 1);
  return 0;
}

