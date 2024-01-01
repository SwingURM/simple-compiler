%option noyywrap
%{
#include<stdio.h>
int ntoken = 0;

void printer(const char* name) {
    printf("%d %s\n", ++ntoken, name);
}
%}

DIGIT [0-9]
LETTER [a-zA-Z]
UNSIGNED {DIGIT}+
COMMENT (\/\*(.|\n)*\*\/)|(\/\/.*)
VAR {LETTER}({DIGIT}|{LETTER}|_)*
REAL {DIGIT}*\.{DIGIT}+

%%
{REAL} { printer("real"); }
{COMMENT} { printer("comment"); }
{UNSIGNED}  { printer("literal"); }
; { printer(";"); }
, { printer(","); }
\( { printer("("); }
\) { printer(")"); }
\. { printer("."); }
const { printer("const"); }
var { printer("var"); }
procedure { printer("procedure"); }
begin { printer("begin"); }
end { printer("end"); }
if { printer("if"); }
call { printer("call"); }
while { printer("while"); }
do { printer("do"); }
read { printer("read"); }
write { printer("write"); }
odd { printer("odd"); }
\+ { printer("+"); }
- { printer("-"); }
\* { printer("*"); }
\/ { printer("/"); }
else { printer("else"); }
for { printer("for"); }
= { printer("="); }
\# { printer("#"); }
> { printer(">"); }
:= { printer(":="); }
"<" { printer("<"); }
">=" { printer(">="); }
"<=" { printer("<="); }
{VAR} { printer("id"); }
[ \t\r\n]+ {}
%%

int main()
{
  yylex();
  return 0;
}