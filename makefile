all:
	clean myshell

clean:
	rm -f *.o myshell

myshell: myshell.c
	gcc -g -m32 -c -o myshell.o myshell.c
	gcc -g -m32 -c -o lineParser.o lineParser.c
	gcc -g -m32 -o myshell myshell.o lineParser.o
