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