all: clean link

clean:
	rm -f *.o virusDetector
	
link: compile
	gcc -g -o virusDetector virusDetector.o

compile: virusDetector.c
	gcc -g -c -o virusDetector.o virusDetector.c