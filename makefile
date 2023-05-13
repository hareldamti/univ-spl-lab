name = task1

all: clean compile link

clean:
	rm -f *.o $(name)

compile:
	nasm -g -f elf32 start.s -o start.o

link:
	gcc -g -w -m32 start.o -o $(name) 