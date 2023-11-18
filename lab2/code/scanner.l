%{
#define YYSTYPE char *
#include "parser.tab.h"
%}

STRING	[a-zA-Z]
DIGIT	[0-9]
ID	    [/a-zA-Z0-9._-]+

%%
<<EOF>>                                 {return 0;}
"&"|";"|"|"             			    {return yytext[0];}
">"|"<"                                 {return yytext[0];}
"\n"						            {return yytext[0]; }
[ ]*					                {}
{ID}+				    		        {yylval = strdup(yytext); return T_ARG; }
.						                {printf("lex err:[%s]", yytext); return 0; }

%%

int yywrap() {
        return 1;
}

