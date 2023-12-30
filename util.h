#ifndef UTIL_H
#define UTIL_H
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "codegen.h"
#include "symbol.h"
#define MAXNSTRING 100
#define STACK_SIZE 1000
#define backpatch(x) instructs[x].op = nInstructs

enum REL_OP { REL_EQ, REL_NEQ, REL_LT, REL_GT, REL_LTE, REL_GTE };

struct constDef {
  char* name;
  int val;
};
extern struct VarDecs* varDecHead;

struct backpatch {
    int ins1, ins2, ins3, insfalse;
};

// global
extern int nLevel;

//
extern int stack[STACK_SIZE];
extern int nStack;
void push(int);
int pop();

void call_block(int);
void ecall_block();

void call_stmt(int);
void ecall_stmt();

// Block generate related
extern int nCurrentLevelAddress, nCurrentLevelFirstInstructAddress, curfunid;
// var dec
extern bool declaringVar;
// write
extern bool writing;
// read
extern bool reading;

#endif