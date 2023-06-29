#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <elf.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <math.h>

# define MEM_BUFFER_SIZE 10000
# define FILENAME_LENGTH 100

void close(int fd);

typedef struct {
  char debug_mode;
  int current_fd;
  int fd[2];
  char filename[2][FILENAME_LENGTH];
  long filesize[2];
  void* map_start[2];
  char quit_signal;
} state;

void toggle_debug_mode(state* s) {
    s->debug_mode = !s->debug_mode;
    printf("Debug flag %s\n", s->debug_mode ? "on" : "off");
}

void examine_elf_file(state* s) {
    if (s->fd[s->current_fd] != -1) {
        close(s->fd[s->current_fd]); // Close previous file
        munmap(s->map_start[s->current_fd], s->filesize[s->current_fd]);
    }
    Elf32_Ehdr elf_hdr;
    int newline;

    printf("enter filename: ");
    fgets(s->filename[s->current_fd], FILENAME_LENGTH, stdin);
    for (newline = 0; s->filename[s->current_fd][newline]; newline++);
    s->filename[s->current_fd][newline - 1] = 0;

    FILE* fp = fopen(s->filename[s->current_fd], "r");
    if (fp == 0) {
        printf("Couldn't open file\n");
        s->fd[s->current_fd] = -1;
        return;
    }
    fseek(fp, 0, SEEK_END);
    s->filesize[s->current_fd] = ftell(fp);
    fclose(fp);

    

    s->fd[s->current_fd] = open(s->filename[s->current_fd], O_RDONLY);
    s->map_start[s->current_fd] = mmap(0, s->filesize[s->current_fd], PROT_READ, MAP_PRIVATE, s->fd[s->current_fd], 0);
    elf_hdr = *(Elf32_Ehdr*)s->map_start[s->current_fd];
    
    printf("First 3 bytes of the magic number: %x %x %x\n", elf_hdr.e_ident[1], elf_hdr.e_ident[2], elf_hdr.e_ident[3]);
    if (memcmp(elf_hdr.e_ident+1, "ELF", 3) != 0) {
        printf("Examined file is not in ELF format!\n");
        munmap(s->map_start[s->current_fd], s->filesize[s->current_fd]);
        close(s->fd[s->current_fd]);
        s->fd[s->current_fd] = -1;
        return;
    }

    printf("Data encoding: %s\nEntry point: 0x%x\nSection Header Table Offset: 0x%x\nSection Header Entries: %d\nSection Header Entry Size: %d\nProgram Header Table Offset: 0x%x\nProgram Header Entries: %d\nProgram Header Entry Size: %d\n",
            elf_hdr.e_ident[5] == 2 ? "2's complement, Big endian" : "2's complement, Little endian",
            elf_hdr.e_entry,
            elf_hdr.e_shoff,
            elf_hdr.e_shnum,
            sizeof(Elf32_Shdr),
            elf_hdr.e_phoff,
            elf_hdr.e_phnum,
            sizeof(Elf32_Phdr)
            );

    s->current_fd = 1 - s->current_fd;
    
}

void print_section_names(state* s) {
    for (int idx = 0; idx < 2; idx++) {
        if (s->fd[idx] == -1) continue;
        printf("Section header table for %s:\n", s->filename[idx]);
        Elf32_Ehdr elf_hdr = *(Elf32_Ehdr*)s->map_start[idx];
        
        if (elf_hdr.e_shstrndx == SHN_UNDEF) {
            continue; // TODO: something?
        }
        Elf32_Shdr shstr_hdr = *(Elf32_Shdr *)(s->map_start[idx] + elf_hdr.e_shoff + sizeof(Elf32_Shdr) * elf_hdr.e_shstrndx);
        unsigned int offset, shstr_offset = (unsigned int)s->map_start[idx] + shstr_hdr.sh_addr + shstr_hdr.sh_offset;
        
        for (int i = 0; i < elf_hdr.e_shnum; i++)
        {
            offset = (unsigned int) s->map_start[idx] + elf_hdr.e_shoff + i * sizeof(Elf32_Shdr);
            Elf32_Shdr shdr = *(Elf32_Shdr*)offset;
            printf("[%d] %-10s\t0x%x\t0x%x\t%d\t%d\n",
                i,
                (char*)(shstr_offset + shdr.sh_name),
                shdr.sh_addr,
                shdr.sh_offset,
                shdr.sh_size,
                shdr.sh_type
            );
        }
    }
}

void print_symbols(state* s) {
    
    for (int idx = 0; idx < 2; idx++) {
        if (s->fd[idx] == -1) continue;
        printf("Symbol table for %s:\n", s->filename[idx]);
        // find strtab, symtab
        
        Elf32_Shdr strtab_hdr, symtab_hdr;
        Elf32_Ehdr elf_hdr = *(Elf32_Ehdr*)s->map_start[idx];
        Elf32_Shdr shstr_hdr = *(Elf32_Shdr *)(s->map_start[idx] + elf_hdr.e_shoff + sizeof(Elf32_Shdr) * elf_hdr.e_shstrndx);
        void* shstr_offset = s->map_start[idx] + shstr_hdr.sh_addr + shstr_hdr.sh_offset;
        unsigned int offset;
                     
        for (int i = 0; i < elf_hdr.e_shnum; i++)
        {
            offset = (unsigned int) s->map_start[idx] + elf_hdr.e_shoff + i * sizeof(Elf32_Shdr);
            Elf32_Shdr* shdr = (Elf32_Shdr*)offset;
            if (shdr->sh_type == SHT_STRTAB) strtab_hdr = *shdr;
            if (shdr->sh_type == SHT_SYMTAB) symtab_hdr = *shdr;
        }
        void* strtab_offset = s->map_start[idx] + strtab_hdr.sh_addr + strtab_hdr.sh_offset;

        // print symbol table using strtab for names
        for (int i = 0; i < symtab_hdr.sh_size / symtab_hdr.sh_entsize; i++)
        {
            offset = (unsigned int) s->map_start[idx] + symtab_hdr.sh_addr + symtab_hdr.sh_offset + i * sizeof(Elf32_Sym);
            Elf32_Sym sym = *(Elf32_Sym*)offset;
            
            printf("[%-2d] 0x%08x\t%6d\t", i, sym.st_value, sym.st_shndx);
            if (sym.st_shndx == SHN_ABS) printf("%14s","ABS");
            else if (sym.st_shndx == SHN_UNDEF) printf("%14s","UNDEF");
            else {
                Elf32_Shdr shdr = *(Elf32_Shdr*)(s->map_start[idx] + elf_hdr.e_shoff + sym.st_shndx * sizeof(Elf32_Shdr));
                printf("%14s\t",(char*)(shstr_offset + shdr.sh_name));
            }
            
            printf("%-10s\n", (char*)(strtab_offset + sym.st_name));
        }
    }
}

void check_files_for_merge(state* s) {
    
    for (int idx = 0; idx < 2; idx++) 
        if (s->fd[idx] == -1) {
            printf("Unsuccessful. Please load 2 files first\n");
            return;
        }

    Elf32_Shdr strtab_hdr[2], symtab_hdr[2];
    for (int idx = 0; idx < 2; idx++) {
        Elf32_Ehdr elf_hdr = *(Elf32_Ehdr*)s->map_start[idx];
        unsigned int offset;
        for (int i = 0; i < elf_hdr.e_shnum; i++)
        {
            offset = (unsigned int) s->map_start[idx] + elf_hdr.e_shoff + i * sizeof(Elf32_Shdr);
            Elf32_Shdr* shdr = (Elf32_Shdr*)offset;
            if (shdr->sh_type == SHT_STRTAB) strtab_hdr[idx] = *shdr;
            if (shdr->sh_type == SHT_SYMTAB) symtab_hdr[idx] = *shdr;
        }
    }

    int is_valid = 1;

    for (int i = 1; i < symtab_hdr[0].sh_size / symtab_hdr[0].sh_entsize; i++) {
        Elf32_Sym sym1 = *(Elf32_Sym*)(s->map_start[0] + symtab_hdr[0].sh_addr + symtab_hdr[0].sh_offset + i * sizeof(Elf32_Sym));
        char* sym1_name = (char *)(s->map_start[0] + strtab_hdr[0].sh_addr + strtab_hdr[0].sh_offset + sym1.st_name);
        int def1 = (sym1.st_shndx != 0) ? 1 : 0;
        for (int j = 1; j < symtab_hdr[1].sh_size / symtab_hdr[1].sh_entsize; j++) {
            Elf32_Sym sym2 = *(Elf32_Sym*)(s->map_start[1] + symtab_hdr[1].sh_addr + symtab_hdr[1].sh_offset + j * sizeof(Elf32_Sym));
            char* sym2_name = (char *)(s->map_start[1] + strtab_hdr[1].sh_addr + strtab_hdr[1].sh_offset + sym2.st_name);
            if (strcmp(sym1_name, sym2_name) == 0 && sym2.st_shndx != 0) {def1++;}
        }
        if (def1 == 0) {
            printf("Symbol %s undefined\n", sym1_name);
            is_valid = 0;
        }
        if (def1 == 2) {
            printf("Symbol %s multiply defined\n", sym1_name);
            is_valid = 0;
        }
    }

    if (is_valid) printf("Files %s and %s have been checked for merger\n",
        s->filename[0], s->filename[1]);
}

void merge_elf_files(state* s) {
    FILE* fp = fopen("out.ro" , "w+");
    Elf32_Ehdr elf_hdr[2];
    Elf32_Shdr* shdr[2];
    Elf32_Shdr* strtab[2];
    Elf32_Shdr* symtab[2];
    char* shstrtab[2];

    // Find symbol, string and section header tables,
    for (int idx = 0; idx < 2; idx++) {
        memcpy(&elf_hdr[idx],s->map_start[idx],sizeof(Elf32_Ehdr));
        Elf32_Shdr shstr_hdr = *(Elf32_Shdr *)(s->map_start[idx] + elf_hdr[idx].e_shoff + sizeof(Elf32_Shdr) * elf_hdr[idx].e_shstrndx);
        shstrtab[idx] = s->map_start[idx] + shstr_hdr.sh_addr + shstr_hdr.sh_offset;

        shdr[idx] = (Elf32_Shdr*)(s->map_start[idx] + elf_hdr[idx].e_shoff);
        for (int i = 0; i < elf_hdr[idx].e_shnum; i++) {
            if (shdr[idx][i].sh_type == SHT_SYMTAB) {symtab[idx] = &shdr[idx][i];}
            if (shdr[idx][i].sh_type == SHT_STRTAB) {strtab[idx] = &shdr[idx][i];}
        }
    }
    
    // Create copies of the 1st section header table, and the symbol table.
    Elf32_Shdr new_sh_tab[elf_hdr[0].e_shnum];
    int n_symbols = symtab[0]->sh_size / symtab[0]->sh_entsize;
    Elf32_Sym new_sym_tab[n_symbols];
    memcpy(new_sh_tab, shdr[0], elf_hdr[0].e_shnum * sizeof(Elf32_Shdr));
    memcpy(new_sym_tab, s->map_start[0] + symtab[0]->sh_addr + symtab[0]->sh_offset, n_symbols * sizeof(Elf32_Sym));


    // Update the copied symbol table with undef shndx with the 2nd file symbol table
    for (int i = 1; i < n_symbols; i++) {
        Elf32_Sym* sym1 = &new_sym_tab[i];
        char* sym1_name = (char *)(s->map_start[0] + strtab[0]->sh_addr + strtab[0]->sh_offset + sym1->st_name);
        if (sym1->st_shndx == 0) {
            for (int j = 1; j < symtab[1]->sh_size / symtab[1]->sh_entsize; j++) {
                Elf32_Sym sym2;
                memcpy(&sym2, (Elf32_Sym*)(s->map_start[1] + symtab[1]->sh_addr + symtab[1]->sh_offset + j * sizeof(Elf32_Sym)), sizeof(Elf32_Sym));
                char* sym2_name = (char *)(s->map_start[1] + strtab[1]->sh_addr + strtab[1]->sh_offset + sym2.st_name);
                if (strcmp(sym1_name, sym2_name) == 0 && sym2.st_shndx != 0) {
                    // find the appropriate section index in the first table for the name from the second table
                    char* section_name = shstrtab[1] + shdr[1][sym2.st_shndx].sh_name;
                    for (int k = 0; k < elf_hdr[0].e_shnum; k++) {
                        if (strcmp(section_name, shstrtab[0] + shdr[0][k].sh_name) == 0) {
                            sym1->st_shndx =  k;
                            sym1->st_value += sym2.st_value;
                            sym1->st_other =  sym2.st_other;
                            sym1->st_size  =  sym2.st_size;
                            sym1->st_info  =  sym2.st_info;
                            break;
                        }
                    }
                }
            }
        }
    }
    
    // Write the sections' content into the file, concatenating the 2 files for the .text, .data, .rodata
    char* concat_sh[3] = {".text", ".data", ".rodata"}; 
    fseek(fp, sizeof(Elf32_Ehdr) + elf_hdr[0].e_shnum * sizeof(Elf32_Shdr), SEEK_SET);
    for (int i = 0; i < elf_hdr[0].e_shnum; i++) {
        unsigned int start = ftell(fp);
        if (new_sh_tab[i].sh_type == SHT_SYMTAB) {
            fwrite(&new_sym_tab, 1, new_sh_tab[i].sh_size, fp);
        }
        else {
            fwrite(s->map_start[0] + shdr[0][i].sh_addr + shdr[0][i].sh_offset, 1, shdr[0][i].sh_size, fp);
            for (int c = 0; c < 3; c++) {
                if (strcmp(shstrtab[0] + shdr[0][i].sh_name, concat_sh[c]) == 0) {
                    for (int j = 0; j < elf_hdr[1].e_shnum; j++) {
                        if (strcmp(shstrtab[1] + shdr[1][j].sh_name, concat_sh[c]) == 0) {
                            fwrite(s->map_start[1] + shdr[1][j].sh_addr + shdr[1][j].sh_offset, 1, shdr[1][j].sh_size, fp);
                        }
                    }
                }
            }
        }
        unsigned int end = ftell(fp);
        new_sh_tab[i].sh_addr = 0;
        new_sh_tab[i].sh_offset = start;
        new_sh_tab[i].sh_size = end - start;
    }
    
    // Update section header offset
    elf_hdr[0].e_shoff += sizeof(Elf32_Ehdr) - elf_hdr[0].e_shoff;

    // Write the elf Header and section header table in the start of the file
    fseek(fp, 0, SEEK_SET);
    fwrite(&elf_hdr[0], 1, sizeof(Elf32_Ehdr), fp);
    fwrite(&new_sh_tab[0], 1, elf_hdr[0].e_shnum * sizeof(Elf32_Shdr), fp);
    fclose(fp);

    
}

void quit(state* s) {
    printf("Quitting.\n");
    s->quit_signal = 1;
}

typedef struct func_desc {
    char *name;
    void (*fun)(state*);
} func_desc;

func_desc menu[] = {
    { "Toggle Debug Mode", &toggle_debug_mode },
    { "Examine ELF File", &examine_elf_file },
    { "Print Section Names", &print_section_names },
    { "Print Symbols", &print_symbols },
    { "Check Files for Merge", &check_files_for_merge },
    { "Merge ELF Files", &merge_elf_files },
    { "Quit", &quit },
    { NULL, NULL }
};

int main(int argc, char **argv) {
    int menu_size = 0, menu_choice;
    while (menu[menu_size].name != NULL) menu_size++;

    state appState;
    appState.debug_mode = appState.current_fd = appState.quit_signal = 0;
    appState.fd[0] = appState.fd[1] = -1;

    do {
        if (appState.debug_mode) {
            // TODO: print debug info
        }

        fprintf(stdout, "Choose action (ctrl^D for exit):\n");
        for (int i = 0; i < menu_size; i++)
            fprintf(stdout, "%d) %s\n", i, menu[i].name);
        fprintf(stdout, "Choice: ");

        if ((menu_choice = fgetc(stdin)) == EOF) break;
        fgetc(stdin); // clear newline input
        int choice = menu_choice - '0';
        if (choice < 0 || choice > menu_size - 1) {
            fprintf(stdout, "Not within bounds\n");
            break;
        }
        (*menu[choice].fun)(&appState);
        if (appState.quit_signal) break;
    }
    while(1);
    for (int i = 0; i < 2; i++) {
        if (appState.fd[i] != -1){
            close(appState.fd[i]);
            munmap(appState.map_start[i], appState.filesize[i]);
        }
    }
    exit(0);
}