#include <stdio.h>

int digit_cnt(char* c){
    int count = 0;
    while (*c != '\0') {
        if (*c >= '0' && *c <= '9') count++;
        c++;
    }
    return count;
}

int main (int ac, char** av) {printf("number: %d",digit_cnt(*(av+1)));}