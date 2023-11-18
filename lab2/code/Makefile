all:
	flex scanner.l
	bison -d parser.y
	gcc -g ast.c lex.yy.c parser.tab.c main.c -o shell

clean:
	rm -rf lex.yy.c parser.tab.h parser.tab.c shell *.s
