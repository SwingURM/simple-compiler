%option noyywrap
%{
#include "util.h"
#include "mybison.tab.h"
%}

DIGIT [0-9]
LETTER [a-zA-Z]
UNSIGNED {DIGIT}+
COMMENT (\/\*(.|\n)*\*\/)|(\/\/.*)
VAR {LETTER}({DIGIT}|{LETTER}|_)*

%%
{COMMENT} {}
{UNSIGNED}  { yylval.val = atoi(yytext); return INTLITERAL; }
; { return SEMICOLON; }
, { return COMMA; }
\( { return LPAR; }
\) { return RPAR; }
\. { return PERIOD; }
const { return CONST; }
var { return VAR; }
procedure { return PROCEDURE; }
begin { return BBEGIN; }
end { return END; }
if { return IF; }
then { return THEN; }
call { return CALL; }
while { return WHILE; }
do { return DO; }
read { return READ; }
write { return WRITE; }
odd { return ODD; }
\+ { return ADD; }
- {return SUB;}
\* {return MUL;}
\/ {return DIV;}
else {return ELSE;}
for {return FOR;}
= {return EQ; }
\# {return NEQ;}
> {return GT; }
:= {return ASSIGN;}
"<" { return LT;}
">=" {return GTE;}
"<=" {return LTE;}
\[ { return LPARM; }
\] { return RPARM; }
: { return COLON; }
{VAR} { yylval.name = strdup(yytext); return IDENT; }
[ \t\n\r]+ {}
%%
