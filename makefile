all: clean compile link

clean:
	rm -f *.o loader

compile:
	gcc -m32 -c loader.c -o loader.o
	nasm -g -f elf32 start.s -o start.o
	nasm -g -f elf32 startup.s -o startup.o
	

link:
	ld -o loader loader.o startup.o start.o -L/usr/lib32 -lc -T linking_script -dynamic-linker /lib32/ld-linux.so.2