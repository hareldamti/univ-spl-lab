all: clean compile link

clean:
	rm -f *.o task1

compile:
	nasm -f elf32 start.s -o start.o
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector util.c -o util.o
	

link:
	ld -m elf_i386 start.o util.o -o task1
