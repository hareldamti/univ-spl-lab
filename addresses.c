﻿#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int addr5;
int addr6;

int foo()
{
    return -1;
}
void point_at(void *p);
void foo1();
char g = 'g';
void foo2();

int main(int argc, char **argv)
{ 
    int addr2;
    int addr3;
    char *yos = "ree";
    int *addr4 = (int *)(malloc(50));
    
    printf("Print addresses:\n");
    printf("- &addr2: %p\n", &addr2);
    printf("- &addr3: %p\n", &addr3);
    printf("- foo: %p\n", &foo);
    printf("- &addr5: %p\n", &addr5);

    printf("- argc %p\n", &argc);
    printf("- argv %p\n", argv);
    printf("- &argv %p\n", &argv);
    
    printf("Print distances:\n");
    point_at(&addr5);

    printf("Print more addresses:\n");
    printf("- &addr6: %p\n", &addr6);
    printf("- yos: %p\n", yos);
    printf("- gg: %p\n", &g);
    printf("- addr4: %p\n", addr4);
    printf("- &addr4: %p\n", &addr4);

    printf("- &foo1: %p\n", &foo1);
    printf("- &foo1: %p\n", &foo2);
    
    printf("Print another distance:\n");
    printf("- &foo2 - &foo1: %ld\n", (long) (&foo2 - &foo1));

   
    printf("Arrays Mem Layout (T1b):\n");
    int iarray[3];
    float farray[3];
    double darray[3];
    char carray[3];
    printf("iarray: %p, iarray+1: %p\t sizeof(int): %d\n", iarray, iarray+1, sizeof(int));
    printf("farray: %p, farray+1: %p\t sizeof(float): %d\n", farray, farray+1, sizeof(float));
    printf("darray: %p, darray+1: %p\t sizeof(double): %d\n", darray, darray+1, sizeof(double));
    printf("carray: %p, carray+1: %p\t sizeof(char): %d\n", carray, carray+1, sizeof(char));
    
    
    /* task 1 b here */
    
    printf("Pointers and arrays (T1d): \n");
    int iarray2[] = {1,2,3};
    char carray2[] = {'a','b','c'};
    int* iarray2Ptr;
    char* carray2Ptr; 
    int* p;
    /* task 1 d here */
    iarray2Ptr = iarray2;
    carray2Ptr = carray2;
    printf("iarray2: [%d, %d, %d]\n", *iarray2Ptr, *(iarray2Ptr+1), *(iarray2Ptr+2));
    printf("carray2: [%c, %c, %c]\n", *carray2Ptr, *(carray2Ptr+1), *(carray2Ptr+2));
    printf("Command line arg addresses (T1e):\n");
    printf("p: %d\n", p);
    /* task 1 e here */
    for (int i = 1; i < argc; i++) 
        printf("\taddr: %p\tvalue: %s\n", argv[i], argv[i]);
    return 0;
}

void point_at(void *p)
{
    int local;
    static int addr0 = 2;
    static int addr1;

    long dist1 = (size_t)&addr6 - (size_t)p;
    long dist2 = (size_t)&local - (size_t)p;
    unsigned long dist2u = (size_t)&local - (size_t)p;
    long dist3 = (size_t)&foo - (size_t)p;

    printf("- dist1: (size_t)&addr6 - (size_t)p: %ld\t\t\t\t addresses: %p - %p\n", dist1, &addr6, p);
    printf("- dist2: (size_t)&local - (size_t)p: %ld\t\t addresses: %p - %p\n", dist2, &local, p);
    printf("- dist2 (unsigned): (size_t)&local - (size_t)p: %lu\t addresses: %p - %p\n", dist2u, &local, p);
    printf("- dist3: (size_t)&foo - (size_t)p:  %ld\t\t\t addresses: %p - %p\n", dist3, &foo, p);
    
    printf("Check long type mem size (T1a):\n");
    /* part of task 1 a here */
    printf("size of int: %d\n", sizeof(int));
    printf("size of long: %d\n", sizeof(long));
    printf("size of long long: %d\n", sizeof(long long));
    // long size was not enough for dist. Unsigned long works
    printf("- addr0: %p\n", &addr0);
    printf("- addr1: %p\n", &addr1);
}

void foo1()
{
    printf("foo1\n");
}

void foo2()
{
    printf("foo2\n");
}
