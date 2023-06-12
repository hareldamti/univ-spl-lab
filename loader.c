#include <stdio.h>
#include <stdlib.h>
#include <elf.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *,int), int arg) {
    Elf32_Ehdr* elf_hdr = (Elf32_Ehdr*) map_start;
    for (int i = 0; i < elf_hdr -> e_phnum; i++) {
        Elf32_Phdr* phdr = (Elf32_Phdr*) (map_start + elf_hdr -> e_phoff + i * sizeof(Elf32_Phdr));
        func(phdr, arg);
    }
}

int print_cntr = 0;
void print(Elf32_Phdr * phdr, int arg) {
    printf("Program header number %d is in address %x\n", print_cntr, phdr -> p_offset);
    print_cntr++;
}

void phdr_table(Elf32_Phdr * phdr, int newline) {
    char flags[3] = {'R', 'W', 'E'};
    for (int i = 0; i < 3; i++) if (!((phdr -> p_flags >> (2-i)) & 1)) flags[i] = ' ';
    printf("%-9d\t%x\t%x\t\t%x\t\t%x\t%x\t%s\t%x%s",
        phdr -> p_type,
        phdr -> p_offset,
        phdr -> p_vaddr,
        phdr -> p_paddr,
        phdr -> p_filesz,
        phdr -> p_memsz,
        flags,
        phdr -> p_align,
        newline ? "\n" : ""
    );
    
}

void load_phdr(Elf32_Phdr * phdr, int fd) {
    if (phdr -> p_type != 1) return;
    int permissions = (phdr -> p_flags & 4)
                    + (phdr -> p_flags & 2) * 2
                    + (phdr -> p_flags & 1) * 4;
    
    Elf32_Off vaddr   = phdr -> p_vaddr  & 0xfffff000,
              offset  = phdr -> p_offset & 0xfffff000,
              padding = phdr -> p_vaddr   & 0xfff;

    void* mapped = mmap( (void*)vaddr, phdr -> p_memsz + padding, permissions, MAP_PRIVATE | MAP_FIXED, fd, offset);
    phdr_table(phdr, 0); printf("\t%x\n", mapped);
}

int main(int argc, char ** argv) {
    if (argc < 2) exit(1);
    int fd = open(argv[1], 'r');

    Elf32_Ehdr* elf_hdr = (Elf32_Ehdr*)mmap(NULL, sizeof(Elf32_Ehdr), PROT_READ, MAP_PRIVATE, fd, 0);
    void* map = mmap(NULL, elf_hdr -> e_phoff + sizeof(Elf32_Phdr) * elf_hdr -> e_phnum, PROT_READ, MAP_PRIVATE, fd, 0);

    //////// task0:
    // foreach_phdr(map, print, 0);

    //////// task1:
    //printf("Type\t\toffset\tVirtAddr\tPhysAddr\tFileSiz\tMemSiz\tFlg\tAlign\n");
    //foreach_phdr(map, phdr_table, 1);

    //////// task2:
    printf("Type\t\toffset\tVirtAddr\tPhysAddr\tFileSiz\tMemSiz\tFlg\tAlign\n");
    foreach_phdr(map, load_phdr, fd);
    printf("starting at: %x\n", (void *)elf_hdr -> e_entry);
    startup(argc - 1, argv + 1, (void *)elf_hdr -> e_entry);
    close(fd);
}