all: clean myshell mypipeline

clean:
	rm -f *.o myshell mypipeline

myshell: myshell.c
	gcc -g -m32 -c -o myshell.o myshell.c
	gcc -g -m32 -c -o LineParser.o LineParser.c
	gcc -g -m32 -o myshell myshell.o LineParser.o

mypipeline: mypipeline.c
	gcc -g -m32 -c -o mypipeline.o mypipeline.c
	gcc -g -m32 -o mypipeline mypipeline.o