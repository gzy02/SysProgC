%{
//#define YYSTYPE int
#include "ast.h"
#include "parser.tab.h"
extern int yylineno;
extern FILE *yyin;
%}

%option yylineno

DIGIT	[0-9]
ALPHA	[a-zA-Z_]
ID      [a-zA-Z_][a-zA-Z_0-9]*

%%
<<EOF>>                                 {return 0;}
"+"            			                {return yytext[0];}
"-"                                     {return yytext[0];}
"*"                                     {return yytext[0];}
"/"                                     {return yytext[0];}
"="                                     {return yytext[0];}
"("                                     {return yytext[0];}
")"                                     {return yytext[0];}
";"                                     {return yytext[0];}
[ \t\r\n]+					            {}
"print"                                 { return PRINT;}
{ID}                                    {yylval.var = strdup(yytext); return VAR; }
{DIGIT}+                                {yylval.num = atoi(yytext); return NUM; }
.						                {printf("lex err at line %d: unexpected character '%s'\n", yylineno, yytext); exit(1); }

%%

int yywrap(){
    return 1;
}

