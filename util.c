#include "util.h"

int nLevel = 0;
int nCurrentLevelAddress, nCurrentLevelFirstInstructAddress, curfunid;
bool declaringVar = false, writing = false, reading = false;
int stack[STACK_SIZE];
int nStack;


void push(int val) { stack[nStack++] = val; }

int pop() { return stack[--nStack]; }

void call_block(int level) {
  push(nCurrentLevelAddress);
  push(curfunid);
  push(nCurrentLevelFirstInstructAddress);
  push(nLevel);
  nCurrentLevelAddress = 3;
  curfunid = nSymbol - 1;
  nLevel = level;
}

void ecall_block() {
  nLevel = pop();
  nCurrentLevelFirstInstructAddress = pop();
  curfunid = pop();
  nCurrentLevelAddress = pop();
}

void call_stmt(int level) {
  push(nLevel);
}

void ecall_stmt() {
  nLevel = pop();
}
// a function create a customized error message
void yyerror(const char *s, ...) {
    va_list ap;
    va_start(ap, s);
    fprintf(stderr, "Error: ");
    vfprintf(stderr, s, ap);
    va_end(ap);
    fprintf(stderr, " in line %d\n", yylineno);
    exit(1);
}
void warn_undef(char *name) {
  yyerror("%s is undefined", name);
}

void warn_redecl(char *name) {
  yyerror("%s has been declared before", name);
}

void warn_typemismatch(char *name, int type, int expected_type) {
    yyerror("type mismatch on %s, expected %s, got %s", name, type2str(expected_type), type2str(type));
}

void type_check(int sid, int expected_type) {
  if (symbol_table[sid].type != expected_type) {
    warn_typemismatch(symbol_table[sid].name, symbol_table[sid].type, expected_type);
  }
}

