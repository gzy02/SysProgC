print_main:
# testbench for printf
	gcc -c print_main.c -g -fno-builtin
	gcc -c printf.c -g -fno-builtin
	gcc -c mini-lib.c -g -fno-builtin
	gcc print_main.o mini-lib.o printf.o -o print_main.out

all:
	gcc hello.c

step:
	cpp hello.c -o hello.i
	/usr/lib/gcc/x86_64-linux-gnu/11/cc1 hello.i -o hello.s
	as hello.s -o hello.o
	ld hello.o -e main -o hello.out

notmain:
# first compile the lib
	gcc -c mini-lib.c -g -fno-builtin
	gcc -c notmain.c -g -fno-builtin
	ld notmain.o mini-lib.o -e notmain -o notmain.out -g

clean:
	rm -rf *.i *.s *.o *.out

