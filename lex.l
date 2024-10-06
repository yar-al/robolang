%option noyywrap

%{
    #include "y.tab.h"
    #include "logic/logic.h"
    #include "ast/ast.h"
    void yyerror(const char *);
%}

%%

go  { yylval.i = 0; return COMMAND; }
pick  { yylval.i = 1; return COMMAND; }
look  { yylval.i = 2; return COMMAND; }
drop    { yylval.i = 3; return COMMAND; }
escape { return ESCAPE; }
print  { return PRINT; }

left    { yylval.i = 1; return DIR; }
right    { yylval.i = 2; return DIR; }
up    { yylval.i = 3; return DIR; }
down    { yylval.i = 4; return DIR; }

in  { return IN; }
all { return ALL; }
some { return SOME; }
less { return LESS; }

function { return FUNCKW; }

if  { return IF; }
do  { return DO; }
end { return FUNCEND; }
from { return FROM; }
to { return TO; }
with[\ ]step { return STEP; }
put { return OUTPUT; }

[#].* {
    return COMMENT;
}

[0-9]+(\.[0-9]+(E[0-9]+)*)?  {
    yylval.i = atoi(yytext);
    return VALUE;
}
true|false  {
    yylval.i = (int)Conv::stob(std::string(yytext));
    return VALUE;
}
EMPTY|WALL|BOX|EXIT|UNDEF   {
    yylval.i = (int)Conv::stoc(std::string(yytext));
    return VALUE;
}


INT|CELL|BOOL   {
    yylval.s = yytext;
    return TYPENAME;
}
[a-zA-Z_][a-zA-Z0-9_]*   {
    yylval.s = yytext;
    return VARIABLENAME;
}

[\<\+\-]\=  {
    yylval.s = yytext;
    return ASOPERATOR;
}

[ \t]       {
    
}

[;\n]    {
    return ENDL;
}

[\+\-\&\|\!\=\,\[\]\{\}\/\*\.]   {
    return *yytext;
}

.   {
    std::cerr << std::endl << "\"" << yytext << "\" is ";
    yyerror("Unexpected input");
    return UNEXP;
} 
