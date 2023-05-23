#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

# define MEM_BUFFER_SIZE 10000

typedef struct {
  char debug_mode;
  char file_name[128];
  int unit_size;
  unsigned char mem_buf[MEM_BUFFER_SIZE];
  size_t mem_count;
  char display_mode;
  char quit_signal;
} state;
state programState;

void toggle_debug_mode(state* s) {
    s->debug_mode = !s->debug_mode;
    printf("Debug flag %s\n", s->debug_mode ? "on" : "off");
}

void set_file_name(state* s) {
    printf("enter new file name: ");
    s->quit_signal = (fgets(s->file_name, 100, stdin) == NULL);
    s->file_name[strlen(s->file_name) - 1] = 0;
    if (s->debug_mode) fprintf(stderr, "Debug: file name set to %s\n", s->file_name);
}

void set_unit_size(state* s) {
    printf("enter new unit size: ");
    int parsed_unit_size = fgetc(stdin) - '0';
    fgetc(stdin); // clear newline input
    if (parsed_unit_size != 1 && parsed_unit_size != 2 && parsed_unit_size != 4) {
        printf("Not a valid unit size\n");
        return;
    }
    s->unit_size = parsed_unit_size;
    if (s->debug_mode) fprintf(stderr, "Debug: unit size set to %d\n", s->unit_size);
}

void load_into_memory(state* s) {
    if (s->file_name[0] == 0) {
        printf("Error: Field 'file name' is empty\n"); return;
    }
    FILE * fp;
    if ((fp = fopen(s->file_name, "r")) == 0) {
        printf("Error: Couldn't open file %s\n", s->file_name); return;
    }
    char input[50];
    printf("enter <address> <length> (%d size units): ", s->unit_size);
    fgets(input, 50, stdin);
    
    int address, length;
    sscanf(input, "%x %d", &address, &length);

    if (s->debug_mode) {
        fprintf(stderr, "Loading into memory:\n\tfile: %s\n\taddress: %x\n\tlength (units): %d\n",
            s->file_name,
            address,
            length 
        );
    }

    fseek(fp, address, SEEK_SET);
    fread(s->mem_buf,  s->unit_size, length, fp);
    fclose(fp);
    
    printf("Loaded %d units into memory\n", length);
}

void toggle_display_mode(state* s) {
    s->display_mode = !s->display_mode;
    printf("Debug flag %sdecimal representation\n", s->display_mode ? "on, hexa" : "off, ");
}

void memory_display(state* s) {
    char input[50];
    printf("enter <address> <length> (%d size units): ", s->unit_size);
    fgets(input, 50, stdin);
    
    int address, length;
    sscanf(input, "%x %d", &address, &length);

    printf("%secimal:\n======\n",s->display_mode ? "Hexad" : "D");
    for (int i = address; i < address + length * s->unit_size && i < MEM_BUFFER_SIZE; i += s->unit_size) {
        int tmp = 0;
        memcpy( &tmp, s->mem_buf + i, s->unit_size);
        if (s->display_mode) printf("%x\n", tmp);
        else                 printf("%d\n", tmp);
    }

}

void save_into_file(state* s) {
    if (s->file_name[0] == 0) {
        printf("Error: Field 'file name' is empty\n"); return;
    }
    FILE * fp;
    if ((fp = fopen(s->file_name, "r+")) == 0) {
        printf("Error: Couldn't open file %s\n", s->file_name); return;
    }
    char input[80];
    printf("enter <source-address> <target-location> <length> (%d size units): ", s->unit_size);
    fgets(input, 80, stdin);
    
    int source_address, target_location, length;
    sscanf(input, "%x %x %d", &source_address, &target_location, &length);
    if (source_address == 0) source_address = (int)s->mem_buf;
    fseek(fp, 0, SEEK_END);
    int file_size = ftell(fp) - target_location;
    if (file_size < length * s->unit_size) {
        printf("Error: file size too small for writing\n");
        return;
    }
    fseek(fp, target_location, SEEK_SET);
    fwrite((int *)source_address, s->unit_size, length, fp);
    fclose(fp);
    
    printf("Saved %d units into file\n", length);
}

void memory_modify(state* s) {
    char input[50];
    printf("enter <location> <val>: ");
    fgets(input, 50, stdin);
    
    int location, val;
    sscanf(input, "%x %x", &location, &val);
    if (s->debug_mode) {
        printf("Memory modify:\n\tlocation: %x\n\tval: %x\n",
            location,
            val
        );
    }
    if (location < 0 || location >= MEM_BUFFER_SIZE - s->unit_size) {
        printf("Error: location outside bounds of buffer\n");
        return;
    }
    memcpy(s->mem_buf + location, &val, s->unit_size);
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
    { "Set File Name", &set_file_name },
    { "Set Unit Size", &set_unit_size },
    { "Load Into Memory", &load_into_memory },
    { "Toggle Display Mode", &toggle_display_mode },
    { "Memory Display", &memory_display },
    { "Save Into File", &save_into_file },
    { "Memory Modify", &memory_modify },
    { "Quit", &quit },
    { NULL, NULL }
};

int main(int argc, char **argv) {
    int menu_size = 0, menu_choice;
    while (menu[menu_size].name != NULL) menu_size++;

    state appState;
    appState.debug_mode = appState.mem_count = appState.quit_signal = appState.display_mode = 0;
    appState.unit_size = 1;

    do {
        if (appState.debug_mode) {
            printf("Current state:\n\tunit size: %d\n\tfile name: %s\n\tmem count: %d\n",
                appState.unit_size,
                appState.file_name,
                appState.mem_count
            );
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
    while (1);
    exit(0);
}