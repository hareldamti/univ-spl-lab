objs = start.o util.o main.o

all : task0 task0b

task0 : $(objs)
		ld -m elf_i386 start.o main.o util.o -o task0
start.o : start.s
		nasm -f elf32 start.s -o start.o
util.o : Util.c Util.h
		gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector Util.c -o util.o
main.o : main.c
		gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector main.c -o main.o
clean_task0 :
		rm -f $(objs) task0

task0b : task0b.o
		ld -m elf_i386 task0b.o -o task0b
task0b.o : sub_task0b.s
		nasm -f elf32 sub_task0b.s -o task0b.o
clean_task0b : 
		rm -f task0b.o task0b

clean :
		rm -f $(objs) task0 task0b.o task0b encoder encoder.o

encoder : encoder.o
		gcc -m32 -g -Wall -o encoder encoder.o
encoder.o : encoder.c
		gcc -m32 -g -Wall -c -o encoder.o encoder.c        
clean_encoder : 
		rm -f encoder encoder.o

