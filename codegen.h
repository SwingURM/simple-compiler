#ifndef CODEGEN_H
#define CODEGEN_H
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "code.h"
#define MAX_NUMBER_INSTRUCTS 10000

extern struct INSTRUCT instructs[MAX_NUMBER_INSTRUCTS];

extern int nInstructs;


int GenIns(int, int, int);
void emit_all();
#endif