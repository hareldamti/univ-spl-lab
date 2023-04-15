#include <stdio.h>
#include <stdlib.h>
#include <string.h>

///// LIST & VIRUS /////
typedef struct virus {
unsigned short SigSize;
char virusName[16];
unsigned char* sig;
} virus;

typedef struct link {
    struct link* nextVirus;
    virus* vir;
} link;

virus *readVirus(FILE * fp) {
    char sizeAndName[18];
    if (fread(sizeAndName, sizeof(char), 18, fp) != 18) {return NULL;}
    virus * parsed = (virus *)malloc(sizeof(virus));
    parsed->SigSize = *((short *)sizeAndName);
    memcpy(parsed->virusName, sizeAndName + 2, 16 * sizeof(char));
    parsed->sig = calloc(parsed->SigSize,sizeof(char));
    fread(parsed->sig, sizeof(char), parsed->SigSize, fp);
    return parsed;
}

void printVirus(virus* virus, FILE* output) {
    int i;
    fprintf(output, "Virus name: %s\nVirus size: %d\nsignature:\n",virus->virusName, virus->SigSize);
    for (i = 0; i < virus->SigSize - 1; i++) fprintf(output, "%x ", virus->sig[i]);
    fprintf(output, "%x\n\n", virus->sig[i]);
}

void detect_virus(char *buffer, unsigned int size, link *virus_list) {
    for (int i = 0; i < size; i++) {
        link* curr = virus_list;
        while (curr != NULL) {
            if (curr->vir->SigSize <= size - i &&
                memcmp(buffer + i,curr->vir->sig, curr->vir->SigSize) == 0) {
                printf("--Virus detected--\n\tstart:\t%d\n\tname:\t%s\n\tsize:%d\n",
                i, curr->vir->virusName, curr->vir->SigSize);
            }
            curr = curr->nextVirus;
        }
    }
}

void neutralize_virus(char *fileName, int signatureOffset) {

}

void list_print(link* virus_list, FILE* output) {
    link* curr = virus_list;
    while (curr != NULL) {
        printVirus(curr->vir, output);
        curr = curr->nextVirus;
    }
}

link* list_append(link* virus_list, virus* data) {
    link* new_list = (link*)malloc(sizeof(link));
    if (virus_list == NULL) {
        new_list->vir = data;
        new_list->nextVirus = NULL;
    }
    else {
        new_list->vir = data;
        new_list->nextVirus = virus_list;
    }
    return new_list;
}

void list_free(link* virus_list) {
    link *curr = virus_list, *next;
    while (curr != NULL) {
        next = curr->nextVirus;
        free(curr->vir->sig);
        free(curr->vir);
        free(curr);
        curr = next;
    }
}


///// CONSTANTS & GLOBALS /////
const short MAX_INPUT_LENGTH = 30; //'load signatures' dependency
static link* virus_list;
static short prior_input = 0;
static char* sus_file_name;
static char sus_file_data[10000]; 
static int sus_file_size;

///// MENU COMPONENTS /////
void load_signatures() {
    printf("Enter a signatures filename to load: ");
    char filename[MAX_INPUT_LENGTH];
    fscanf(stdin, "%29s", filename);
    prior_input = 1;
    FILE * fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Couldn't open file %s",filename);
        exit(0);
    }
    fseek(fp, 4, SEEK_CUR); // Still needed or removed from test files?
    virus* v;
    while ((v = readVirus(fp)) != NULL) {
        virus_list = list_append(virus_list, v);
    }
    fclose(fp);
}

void print_signatures() {
    list_print(virus_list, stdout);
}

void detect_viruses() {
    FILE * fp = fopen(sus_file_name, "r");
    fread(sus_file_data, sizeof(char), 10000, fp);
    sus_file_size = ftell(fp);
    detect_virus(sus_file_data, sus_file_size, virus_list);
    fclose(fp);
}

void fix_file() {
    FILE * fp = fopen(sus_file_name, "r+");
    for (int i = 0; i < sus_file_size; i++) {
        link* curr = virus_list;
        while (curr != NULL) {
            if (curr->vir->SigSize <= sus_file_size - i &&
                memcmp(sus_file_data + i,curr->vir->sig, curr->vir->SigSize) == 0) {
                fseek(fp, i, SEEK_SET);
                fwrite("\xC3", sizeof(char), 1, fp);
            }
            curr = curr->nextVirus;
        }
    }
    fclose(fp);
}

void quit() {list_free(virus_list); exit(0);}

typedef struct func_pair {
    char *name;
    void (*fun)();
} func_pair;

func_pair menu[] = {
    { "Load signatures", &load_signatures },
    { "Print signatures", &print_signatures },
    { "Detect viruses", &detect_viruses },
    { "Fix file", &fix_file },
    { "Quit", &quit},
    { NULL, NULL }
};


int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Reopen: ./virusDetector <filename>");
        exit(0);
    }
    sus_file_name = argv[1];

    int menu_size = 0;
    char input[MAX_INPUT_LENGTH];
    while (menu[menu_size].name != NULL) menu_size++;
    while(1) {
        fprintf(stdout, "Please choose a function (ctrl^D for exit):\n");
        for (int i = 0; i < menu_size; i++)
            fprintf(stdout, "%d) %s\n", i + 1, menu[i].name);
        fprintf(stdout, "Choice: ");
        if (prior_input) { fgets(input, MAX_INPUT_LENGTH, stdin); prior_input = 0; }
        if (fgets(input, MAX_INPUT_LENGTH, stdin) == NULL) break;
        int choice = atoi(input);
        if (choice < 1 || choice > menu_size) {
            fprintf(stdout, "Not within bounds\n");
            break;
        }
        if (1 <= choice <= menu_size) {
            printf("%d\n", choice);
            (*(menu[choice - 1].fun))();
        }
    }
}