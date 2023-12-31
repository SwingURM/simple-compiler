%{
#include "ast.h"
#include "util.h"
#include "symbol.h"
#include "codegen.h"
int yydebug = 1;
%}

%start program
%define parse.trace
%union {
    char op;
    char* name;
    int val;
    struct backpatch bp;
}
%token SEMICOLON LPAR RPAR COMMA PERIOD
%token CONST VAR IF END BBEGIN CALL DO READ WRITE PROCEDURE WHILE ELSE FOR
%token ODD
%token<op> EQ LTE GTE GT LT NEQ
%token ASSIGN
%token<op> ADD SUB MUL DIV
%type<op> RelOp
%type<bp> if_then

%token<name> IDENT
%token<val> INTLITERAL
%left ADD SUB
%left MUL DIV
%precedence NEG

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%%
ConstDecl01:
    | ConstDecl
    ;
VarDecl01:
    | VarDecl
    ;
FuncDecl01:
    | FuncDecl
    ;

listConstDef1: listConstDef1 COMMA ConstDef
    | ConstDef
    ;
idents: idents COMMA IDENT
    {
        if (declaringVar) {
            register_var($3, nLevel, &nCurrentLevelAddress);
            add_ast(nID);
        }
        if (reading) {
            GenIns(iOPR, 0, 16);
            int sid = find_symbol($3);
            if (sid == -1) warn_undef($3);
            type_check(sid, ST_VAR);
            GenIns(iSTO, nLevel - symbol_table[sid].nLevel, symbol_table[sid].addr);
        }
    }
    | IDENT
    {
        if (declaringVar) {
            register_var($1, nLevel, &nCurrentLevelAddress);
            add_ast(nID);
        }
        if (reading) {
            GenIns(iOPR, 0, 16);
            int sid = find_symbol($1);
            if (sid == -1) warn_undef($1);
            type_check(sid, ST_VAR);
            GenIns(iSTO, nLevel - symbol_table[sid].nLevel, symbol_table[sid].addr);
        }
    }
    ;


listStmt1: Stmt
    | listStmt1 SEMICOLON Stmt
    ;
listExp1: Exp { if (writing) {GenIns(iOPR, 0, 14);   GenIns(iOPR, 0, 15); } }
    | listExp1 COMMA Exp {  if (writing) {GenIns(iOPR, 0, 14); GenIns(iOPR, 0, 15); }   }
    ;

program : { init_tree(); symbol_table[0].type = ST_PROC; call_block(0);} Block PERIOD { leave_ast(); }
    ;

Block :
    {
        enter_ast(nBLOCK);
        assert(symbol_table[curfunid].type == ST_PROC);
        symbol_table[curfunid].addr = nInstructs;
        GenIns(iJMP, 0, 0);
    }
    ConstDecl01 VarDecl01 FuncDecl01
    {
        assert(instructs[symbol_table[curfunid].addr].iid == iJMP);
        instructs[symbol_table[curfunid].addr].op = nInstructs;
        assert(symbol_table[curfunid].type == ST_PROC);
        symbol_table[curfunid].addr = nInstructs;
        symbol_table[curfunid].attr.proc_attr.size = nCurrentLevelAddress;

        GenIns(iINT, 0, nCurrentLevelAddress);
    }
    Stmt
    {
        GenIns(iOPR, 0, 0);
        leave_ast(nBLOCK);
        ecall_block();
    }
    ;

if_then:
    IF {enter_ast(nIF_STMT);}
    LPAR CONDITION RPAR
    {   $$.insfalse = GenIns(iJPC, 0, 0);   }
    /* | IF LPAR CONDITION error
    { yyerror("Missing right parenthesis"); } */
    ;
Stmt:conditional_stmt
    |
    other_stmt
    ;

conditional_stmt:FOR LPAR Stmt SEMICOLON
    <bp>{
        $$.ins2 = nInstructs;
    }
    CONDITION
    <bp>{
        $$.insfalse = GenIns(iJPC, 0, 0);
        $$.ins3 = GenIns(iJMP, 0, 0); // goto after the statement between {}
    }
    SEMICOLON
    <bp>{
        $$.ins1 = nInstructs; // after the statement between {}
    } Stmt RPAR
    {
        GenIns(iJMP, 0, $5.ins2); // goto condition
        assert(instructs[$7.ins3].iid == iJMP);
        backpatch($7.ins3);
    }
    Stmt
    {
        GenIns(iJMP, 0, $9.ins1); // goto update
        assert(instructs[$7.insfalse].iid == iJPC);
        backpatch($7.insfalse);
        add_ast(nFOR_STMT);
    }
    | if_then Stmt ELSE
    <bp>{
        $$.ins1 = GenIns(iJMP, 0, 0);
        assert(instructs[$1.insfalse].iid == iJPC);
        backpatch($1.insfalse);
    }
    Stmt
    {
        assert(instructs[$4.ins1].iid == iJMP);
        backpatch($4.ins1);
        leave_ast();
    }
    |if_then Stmt
    {
        assert(instructs[$1.insfalse].iid == iJPC);
        backpatch($1.insfalse);
        leave_ast();
    } %prec LOWER_THAN_ELSE
    /* | IF CONDITION error Stmt { yyerror("Missing then"); } */
    |
    <bp>{
        $$.ins1 = nInstructs;
        enter_ast(nWHILE_STMT);
    }
    WHILE CONDITION DO
    <bp>{
        $$.insfalse = GenIns(iJPC, 0, 0);
    }
    Stmt
    {
        GenIns(iJMP, 0, $1.ins1);
        assert(instructs[$5.insfalse].iid == iJPC);
        backpatch($5.insfalse);
        leave_ast();
    }
    ;
other_stmt:IDENT ASSIGN Exp
    {
        int sid = find_symbol($1);
        if (sid == -1) warn_undef($1);
        type_check(sid, ST_VAR);
        GenIns(iSTO, nLevel - symbol_table[sid].nLevel, symbol_table[sid].addr);
        add_ast(nASSIGN_STMT);
    }
    | CALL IDENT
    {
        int sid = find_symbol($2);
        if (sid == -1) warn_undef($2);
        type_check(sid, ST_PROC);
        GenIns(iCAL, nLevel - symbol_table[sid].nLevel, symbol_table[sid].addr);
        add_ast(nCALL_STMT);
    }
    | READ LPAR
    {   reading = true; }
    idents
    {   reading = false;  add_ast(nREAD_STMT);   }
    RPAR
    | WRITE LPAR
    {   writing = true; }
    listExp1
    {   writing = false; add_ast(nWRITE_STMT);}
    RPAR
    | BBEGIN listStmt1 END
    |
    ;

ConstDecl : { enter_ast(nCONST_DECL); } CONST listConstDef1 SEMICOLON { leave_ast();}
    ;
ConstDef : IDENT EQ INTLITERAL
    { register_const($1, $3);
      add_ast(nID); }
    ;
VarDecl : { declaringVar = true; enter_ast(nVAR_DECL); }
    VAR idents SEMICOLON
    { declaringVar = false; leave_ast();}
    ;

FuncDecl : FuncDecl FuncHead
    {   call_block(nLevel + 1);
        enter_ast(nPROC_DECL); }
    Block
    SEMICOLON
    {   leave_ast();    }
    | FuncHead
    {   call_block(nLevel + 1);
        enter_ast(nPROC_DECL); }
    Block
    SEMICOLON
    {   leave_ast();  }
    ;
FuncHead : PROCEDURE IDENT
    {   register_proc($2, nLevel); }
    SEMICOLON
    ;

CONDITION :Exp RelOp Exp
    {
        switch($2) {
            case REL_EQ:
                GenIns(iOPR, 0, 8);
                break;
            case REL_NEQ:
                GenIns(iOPR, 0, 9);
                break;
            case REL_LT:
                GenIns(iOPR, 0, 10);
                break;
            case REL_GTE:
                GenIns(iOPR, 0, 11);
                break;
            case REL_GT:
                GenIns(iOPR, 0, 12);
                break;
            case REL_LTE:
                GenIns(iOPR, 0, 13);
                break;
        }
    }
    | ODD Exp
    {   GenIns(iOPR, 0, 6); }
    ;

Exp : Exp ADD Exp
    {   GenIns(iOPR, 0, 2); }
    | Exp SUB Exp
    {   GenIns(iOPR, 0, 3); }
    | Exp MUL Exp
    {   GenIns(iOPR, 0, 4); }
    | Exp DIV Exp
    {   GenIns(iOPR, 0, 5); }
    | SUB Exp %prec NEG
    {   GenIns(iOPR, 0, 1); }
    | LPAR Exp RPAR
    | LPAR Exp error { yyerror("Missing right parenthesis"); }
    | IDENT
    {
        int sid = find_symbol($1);
        if (sid == -1) warn_undef($1);
        if (symbol_table[sid].type == ST_VAR) GenIns(iLOD, nLevel - symbol_table[sid].nLevel, symbol_table[sid].addr);
        else if (symbol_table[sid].type == ST_CONST) GenIns(iLIT, 0, symbol_table[sid].attr.const_attr.val);
    }
    | INTLITERAL
    {   GenIns(iLIT, 0, $1);    }
    ;

RelOp : EQ { $$ = REL_EQ; }
    | NEQ { $$ = REL_NEQ; }
    | LTE { $$ = REL_LTE; }
    | GTE { $$ = REL_GTE; }
    | LT { $$ = REL_LT; }
    | GT { $$ = REL_GT; }
    ;
%%

int main(void)
{
    yyparse();
    emit_all();
    print_ast();
    return 0;
}