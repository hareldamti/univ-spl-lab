all : main

main : main.o start.o
		ld -m elf_i386 start.o main.o -o main
start.o : start.s
		nasm -f elf32 start.s -o start.o
main.o : main.c
		gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector main.c -o main.o

clean :
		rm -f main.o start.o main

test : test.o
		ld -m elf_i386 test.o -o test
test.o : test.s
		nasm -f elf32 test.s -o test.o
clean_test : 
		rm -f test.o test