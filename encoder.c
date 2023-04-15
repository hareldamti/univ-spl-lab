#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

void parse_debug_mode(bool* mode, char* arg);
void parse_encoding(int* size, int** vector, char* arg);
void parse_stream(FILE** input, char identifier, char* arg);
int positive_mod(int x, int start, int length);
char encode(char c, int x);

int main(int argc, char* argv[]) {
    bool debug = false; //debug mode
    int encs = 0; //encoding size
    int* encv; //encoding vector
    FILE *infile = stdin;
    FILE *outfile = stdout;

    // parse debug args
    for (int i = 1; i < argc; i++) {
        parse_debug_mode(&debug, argv[i]);
        parse_stream(&infile, 'i', argv[i]);
        parse_stream(&outfile, 'o', argv[i]);
        if (debug) fprintf(stderr, (i == argc - 1)?"%s\n":"%s ", argv[i]);

    }
    for (int i = 1; i < argc; i++) {
        parse_encoding(&encs, &encv, argv[i]);
        //encoding is parsed after stream to avoid data leaks in case of a file error 
    }

    // loop read
    int enc_idx = 0;
    char in_char, out_char;
    while ((in_char = fgetc(infile)) != EOF) {
        if (encs == 0) out_char = in_char;
        else {
            out_char = encode(in_char, encv[enc_idx]);
            enc_idx = (enc_idx + 1) % encs;
        }
        fputc(out_char, outfile);
    }

    if (encs != 0)
        free(encv);
    return 0;
}

void parse_debug_mode(bool* mode, char* arg) {
    if (arg[1] == 'D')
        *mode = arg[0] == '+';
}

void parse_encoding(int* size, int** vector, char* arg) {
    if (arg[1] == 'e') {
        int sign = (arg[0] == '+')?1 : -1;
        for (*size = 0; arg[(*size)+2] != '\0'; (*size)++);
        *vector = malloc(*size * sizeof(int));
        for (int i = 0; i < *size; i++) {
            (*vector)[i] = sign * (arg[i+2] - '0');
        }
    }
}

void parse_stream(FILE** input, char identifier, char* arg) {
    if (arg[0] != '-' || arg[1] != identifier) return;
    char* mode = (identifier == 'i') ? "r" : "w";
    char* file = arg + 2;
    FILE* stream = fopen(file, mode);
    if (stream == NULL) {
        fprintf(stderr, "Couldn't open %s. Exiting...\n", file);
        exit(0);
    }
    else *input = stream;
}

int positive_mod(int x, int start, int length) {
    return ((x - start) % length + length) % length + start;
}

char encode(char c, int x) {
    int ord = (int)c;
    if (ord >= 97 && ord <= 122) // small case
        return (char)positive_mod(ord + x, 97, 26);
    
    if (ord >= 65 && ord <= 90) // capital
        return (char)positive_mod(ord + x, 65, 26);

    if (ord >= 48 && ord <= 57) // digits
        return (char)positive_mod(ord + x, 48, 10);
    
    return c;
}

