#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#define MAX_LENGTH_WORD 20
#define MAX_NUMBER_SYMBOLS 10000

enum sym_type {
    ST_CONST,
    ST_VAR,
    ST_PROC,
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

extern int nSymbol;
void register_symbol(struct tablestruct*, int*);
void register_const(char*, int);
void register_var(char*, int, int*);
void register_proc(char*, int);
void table_printer();
int find_symbol(char *);
#endif  // SYMTABLE_H