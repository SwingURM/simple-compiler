#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#define MAX_LENGTH_WORD 20
#define MAX_NUMBER_SYMBOLS 10000

enum sym_type {
  ST_CONST,
  ST_VAR,
  ST_PROC,
};

static char *sym_type_str[] = {
    [ST_CONST] = "const",
    [ST_VAR] = "var",
    [ST_PROC] = "proc",
};

extern struct tablestruct {
  char name[MAX_LENGTH_WORD];
  enum sym_type type;
  union {
    struct {
      int val;
    } const_attr;
    struct {
      int size;
    } proc_attr;
  } attr;
  int nLevel;
  int addr;
} symbol_table[MAX_NUMBER_SYMBOLS];

extern int nSymbol;
void register_symbol(struct tablestruct *, int *);
void register_const(char *, int);
void register_var(char *, int, int *);
void register_proc(char *, int);
void table_printer();
int find_symbol(char *);
const char *type2str(int type);
#endif  // SYMTABLE_H