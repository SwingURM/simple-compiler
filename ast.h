#ifndef AST_H
#define AST_H
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct tree_struct *tree;
typedef struct list_struct *list;

enum node_type {
  nPROGRAM,
  nBLOCK,
  nCONST_DECL,
  nVAR_DECL,
  nPROC_DECL,
  nSTMT,
  nASSIGN_STMT,
  nIF_STMT,
  nWHILE_STMT,
  nFOR_STMT,
  nCALL_STMT,
  nREAD_STMT,
  nWRITE_STMT,
  nID,
};

extern tree root;

struct info {
  int type;
};

struct list_struct {
  tree node;
  list next;
};

struct tree_struct {
  struct info val;
  list children;
};

void init_tree();  // program
void add_ast(int);
void enter_ast(int);
void leave_ast();
void print_ast();

static const char *node_type_str[] = {[nPROGRAM] = "program",
                                      [nBLOCK] = "block",
                                      [nCONST_DECL] = "const_decl",
                                      [nVAR_DECL] = "var_decl",
                                      [nPROC_DECL] = "proc_decl",
                                      [nSTMT] = "stmt",
                                      [nASSIGN_STMT] = "assign_stmt",
                                      [nIF_STMT] = "if_stmt",
                                      [nWHILE_STMT] = "while_stmt",
                                      [nCALL_STMT] = "call_stmt",
                                      [nREAD_STMT] = "read_stmt",
                                      [nWRITE_STMT] = "write_stmt",
                                      [nID] = "id",
                                      [nFOR_STMT] = "for_stmt(simplified)"};

#endif  // AST_H