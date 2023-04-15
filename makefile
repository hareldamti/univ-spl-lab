all: clean myshell looper mypipe

clean:
	rm -f *.o myshell looper mypipe

myshell: myshell.c
	gcc -g -m32 -c -o myshell.o myshell.c
	gcc -g -m32 -c -o lineParser.o lineParser.c
	gcc -g -m32 -o myshell myshell.o lineParser.o

looper: looper.c
	gcc -g -m32 -c -o looper.o looper.c
	gcc -g -m32 -o looper looper.o

mypipe: mypipe.c
	gcc -g -m32 -c -o mypipe.o mypipe.c
	gcc -g -m32 -o mypipe mypipe.o