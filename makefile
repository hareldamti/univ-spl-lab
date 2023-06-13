all: clean linker

clean:
	rm -f *.o linker

linker: linker.o
	gcc -m32 -g -Wall -o linker linker.o

linker.o: linker.c
	gcc -m32 -c -g -Wall -o linker.o linker.c
