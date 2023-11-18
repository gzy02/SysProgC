%{
//#define YYSTYPE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ast.h"

int yylex();
void yyerror(const char* msg);

extern Stm_t root;
extern int yylineno;

%}

%union{
    int num;
    char *var;
    Stm_t stm;
    Exp_t exp;
}

%token<var> VAR
%token<num> NUM

%token PRINT

%type<stm> program
%type<stm> stm
%type<exp> exp

%left '|' ';'
%left '+' '-'
%left '*' '/'

%%

program     : stm       		   { root = $1; $$ = $1; return 0; }
            ;

stm	        : VAR '=' exp	       { $$ = Stm_Assign_new($1, $3); }
		    | PRINT '(' exp ')'    { $$ = Stm_Print_new($3); }
		    | stm ';' stm          { $$ = Stm_Seq_new($1, $3); }
		    ;
		
exp         : NUM			       { $$ = Exp_Num_new($1); }
		    | VAR                  { $$ = Exp_Var_new($1); }
		    | exp '+' exp          { $$ = Exp_Add_new($1, $3); }
		    | exp '-' exp          { $$ = Exp_Sub_new($1, $3); }
		    | exp '*' exp          { $$ = Exp_Times_new($1, $3); }
		    | exp '/' exp          { $$ = Exp_Div_new($1, $3); }
		    | '(' exp ')'          { $$ = $2; }
		    ;

%%

void yyerror(const char *msg) {
    fprintf(stderr, "Parse error at line %d: %s\n", yylineno, msg);
    exit(1);
}

