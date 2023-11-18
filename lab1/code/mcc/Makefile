all:
	flex scanner.l
	bison -d parser.y
	gcc -g ast.c lex.yy.c parser.tab.c compile-x64.c compile-riscv.c set.c main.c -o mcc

clean:
	rm -rf lex.yy.c parser.tab.h parser.tab.c mcc *.s *.out *.o
