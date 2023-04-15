all: clean compile link

clean:
	rm -f *.o encoder

compile:
	gcc -m32 -g -Wall -c -o encoder.o encoder.c

link:
	gcc -m32 -g -Wall -o encoder encoder.o

run:
	./encoder +D