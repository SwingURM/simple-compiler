#include "symbol.h"

struct tablestruct symbol_table[MAX_NUMBER_SYMBOLS];
int nSymbol = 1;
struct tablestruct temp;

void register_const(char* name, int val) {
  if (find_symbol(name) != -1) {
    warn_redecl(name);
  }
  struct tablestruct* temp = &symbol_table[nSymbol];
  strcpy(temp->name, name);
  temp->type = ST_CONST;
  temp->attr.const_attr.val = val;
  nSymbol++;
}

void register_var(char* name, int level, int* addr) {
  int sid = find_symbol(name);
  if (sid != -1 && symbol_table[sid].type != ST_VAR) {
    warn_redecl(name);
  }
  if (sid != -1)
    fprintf(stderr, "Warning: %s has been declared before\n", name);
  struct tablestruct* temp = &symbol_table[nSymbol];
  strcpy(temp->name, name);
  temp->type = ST_VAR;
  temp->nLevel = level;
  temp->addr = *addr;
  (*addr)++;
  nSymbol++;
}

void register_proc(char* name, int level) {
  if (find_symbol(name) != -1) {
    warn_redecl(name);
  }
  struct tablestruct* temp = &symbol_table[nSymbol];
  strcpy(temp->name, name);
  temp->type = ST_PROC;
  temp->nLevel = level;
  nSymbol++;
}

void table_printer() {
  for (int i = 0; i < nSymbol; i++) {
    switch (symbol_table[i].type) {
      case ST_CONST:
        printf("CST %-10s%-10d\n", symbol_table[i].name,
               symbol_table[i].attr.const_attr.val);
        break;
      case ST_VAR:
        printf("VAR %-10s%-5d%-5d\n", symbol_table[i].name,
               symbol_table[i].nLevel, symbol_table[i].addr);
        break;
      case ST_PROC:
        printf("PRO %-10s%-5d%-5d%-5d\n", symbol_table[i].name,
               symbol_table[i].nLevel, symbol_table[i].addr,
               symbol_table[i].attr.proc_attr.size);
        break;
    }
  }
}

int find_symbol(char* name) {
  for (int i = 0; i < nSymbol; i++) {
    if (strcmp(symbol_table[i].name, name) == 0) return i;
  }
  return -1;
}

const char* type2str(int type) { return sym_type_str[type]; }
