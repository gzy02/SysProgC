%{
//#define YYSTYPE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ast.h"
#include "alloc.h"

int yylex();
void yyerror(const char* msg);

Cmd_t cmd;
extern Cmd_t root;
%}

%union{
    int num;
    char *id;
    Cmd_t cmd;
    node node;
}

%token<id> T_ARG

%type<cmd> command basic_command
%type<num> line
%type<node> args

%left '|' ';'
%left '>' '<'
%left '&'

%%

line            :  command '\n'	    		{ root = $1; $$ = 0; return 0; }
		        | '\n'       		{ root = 0; $$ = 0; return 0; }
                ;
                
command	        :  basic_command		    { $$ = $1;}
		        |  command ';' command  	{ $$ = Cmd_Seq_new($1, $3);}
		        |  command '&'			    { $$ = Cmd_Back_new($1);}
		        |  command '|' command 		{ $$ = Cmd_Pipe_new($1, $3);}
		        |  command '>' command		{ $$ = Cmd_Redir_new($1, $3, 1);}
		        |  command '<' command		{ $$ = Cmd_Redir_new($1, $3, 0);}
		        ;
		
basic_command   :  T_ARG args			    { struct node *t = make_node($1, $2); $$ = Cmd_Atom_new(t);}
		        ;
		
args		    : T_ARG args			    { $$ = make_node($1, $2);}
		        |                           { $$ = (struct node *)0;}
		        ;

%%

void yyerror(const char* msg) {
    printf("%s\n",msg);
    exit(1);
}

