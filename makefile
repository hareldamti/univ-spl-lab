all: clean addresses count-words base menu

clean:
	rm -f *.o count-words addresses base menu

addresses.o: addresses.c
	gcc -m32 -g -c -Wall -o addresses.o addresses.c
addresses: addresses.o
	gcc -m32 -g -Wall -o addresses addresses.o

count-words.o: count-words.c
	gcc -m32 -g -c -Wall -o count-words.o count-words.c
count-words: count-words.o
	gcc -m32 -g -Wall -o count-words count-words.o

base.o: base.c
	gcc -m32 -g -c -Wall -o base.o base.c
base: base.o
	gcc -m32 -g -Wall -o base base.o

menu.o: menu.c
	gcc -m32 -g -c -Wall -o menu.o menu.c
menu: menu.o base.o
	gcc -m32 -g -Wall -o menu menu.o