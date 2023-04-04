#include <stdlib.h>
#include <stdio.h>

void printHex(char* data, int length);

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Rerun with input file as argument\n");
        exit(0);
    }
    char* filename = argv[1];
    FILE* fp = fopen(filename, "r");
    fseek(fp, 0, SEEK_END);
    int filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* data = (char *)calloc(filesize, sizeof(char));
    fread(data, sizeof(char), filesize, fp);
    printHex(data, filesize);
    free(data);
}

void printHex(char* data, int length) {
    int i;
    for (i = 0; i < length - 1; i++) {
        printf("%x ", data[i]);
    }
    printf("%x\n", data[i]);
}