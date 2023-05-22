name = task0

all: clean compile link

clean:
	rm -f *.o $(name)

compile:
	nasm -g -f elf32 $(name).s -o $(name).o

link:
	gcc -no-pie -g -m32 $(name).o -o $(name) 