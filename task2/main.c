#define SYS_OPEN     5
#define SYS_GETDENTS 141
#define SYS_WRITE    4
#define SYS_CLODE    
#define SYS_EXIT     1

#define BUFFER_SIZE  8192

#define O_RDONLY 00
#define O_RDWR          2
#define O_DIRECTORY 0200000
#define S_IRWXU  00400 | 00200
#define STDOUT 1

extern int system_call();

typedef struct linux_dirent {
    unsigned long  d_ino;     /* Inode number */
    unsigned long  d_off;     /* Offset to next linux_dirent */
    unsigned short d_reclen;  /* Length of this linux_dirent */
    char           d_name[];  /* Filename (null-terminated) */
} linux_dirent;


int main(int argc, char** argv) {
    unsigned int fd = system_call(SYS_OPEN, ".", O_RDONLY, S_IRWXU);
    char print[] = {fd+'0','\n'};
    system_call(SYS_WRITE, STDOUT, print, 2);
    char buffer[BUFFER_SIZE];
    long x = system_call(SYS_GETDENTS, fd, buffer, BUFFER_SIZE);
    if (x < 0) {
        x *= -1;
        print[0] = x/10+'0';
        print[1] = x%10+'0';
        system_call(SYS_WRITE, STDOUT, print, 2);
    }
    system_call(SYS_WRITE, STDOUT, buffer, BUFFER_SIZE);
    return 0;
}