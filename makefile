names = task1 task4

all: clean $(names)

clean:
	rm -f *.o $(names)

task1: task1.o
	gcc -m32 -g -Wall -o task1 task1.o

task1.o: task1.c
	gcc -m32 -c -g -Wall -o task1.o task1.c

task4:
	gcc -c -m32 -Wall -o task4.o task4.c
	gcc -m32 -fno-pie -fno-stack-protector -o task4 task4.o