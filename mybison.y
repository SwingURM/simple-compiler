%{
int yylex(void);
void yyerror(const char *s);
#include "util.h"
#include "symbol.h"
#include "codegen.h"
int yydebug = 1;
%}

%start program
%define parse.trace
%union {
    struct constDef cd;
    char op;
    char* name;
    int val;
    struct backpatch bp;
    struct dim dim;
    struct dimcal dc;
}
%type Block ConstDecl VarDecl FuncDecl01 Stmt Exp other_stmt conditional_stmt
%token SEMICOLON LPAR RPAR COMMA PERIOD LPARM RPARM COLON
%token CONST VAR IF THEN END BBEGIN CALL DO READ WRITE PROCEDURE WHILE ELSE FOR
%token ODD
%token<op> EQ LTE GTE GT LT NEQ
%token ASSIGN
%token<op> ADD SUB MUL DIV
%type<op> RelOp
%type<bp> if_then
%type<dim> indexes indexes_lit

%type<cd> ConstDef
%token<name> IDENT
%token<val> INTLITERAL
%type<dc> identt
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

listConstDef1: listConstDef1 COMMA ConstDef { register_const($3.name, $3.val); }
    | ConstDef { register_const($1.name, $1.val); }
    ;
idents: idents COMMA IDENT
    {
        if (reading) {
            GenIns(iOPR, 0, 16);
            int sid = find_symbol($3);
            GenIns(iSTO, nLevel - symbol_table[sid].nLevel, symbol_table[sid].addr);
        }
    }
    | IDENT
    {
        if (reading) {
            GenIns(iOPR, 0, 16);
            int sid = find_symbol($1);
            GenIns(iSTO, nLevel - symbol_table[sid].nLevel, symbol_table[sid].addr);
        }
    }
    | IDENT indexes
    {
        if (reading) {
            GenIns(iOPR, 0, 16);
            int sid = find_symbol($1);
            GenIns(iSTO, nLevel - symbol_table[sid].nLevel, symbol_table[sid].addr);
        }
    }
    ;

identt: IDENT { 
        $$.sid = find_symbol($1);
        $$.depth = 0;
    }
    | identt LPARM Exp RPARM
    {
        $$ = $1;
        int size = array_part_size($$.sid, $$.depth);
        GenIns(iLIT, 0, size);
        GenIns(iOPR, 0, 4);
        GenIns(iOPR, 0, 2);
        $$.depth++;
    }

idents_lit: idents_lit COMMA IDENT
    {
        if (declaringVar) register_var($3, nLevel, &nCurrentLevelAddress);
    }
    | IDENT
    {
        if (declaringVar) register_var($1, nLevel, &nCurrentLevelAddress);
    }
    | IDENT indexes_lit
    {
        if (declaringVar) register_array($1, nLevel, &nCurrentLevelAddress, $2.ndim, $2.dims);
    }
    ;

listStmt1: Stmt
    | listStmt1 SEMICOLON Stmt
    ;
listExp1: Exp { if (writing) {GenIns(iOPR, 0, 14);   GenIns(iOPR, 0, 15); } }
    | listExp1 COMMA Exp {  if (writing) {GenIns(iOPR, 0, 14); GenIns(iOPR, 0, 15); }   }
    ;

indexes: indexes LPARM Exp RPARM
    {
        // $$ = $1;
    }
    | LPARM Exp RPARM
    {
        // $$.ndims = 1;
    }
    ;

indexes_lit: indexes LPARM COLON INTLITERAL RPARM
    {
        $$ = $1;
        $$.dims[$$.ndim++] = $4;
    }
    | LPARM COLON INTLITERAL RPARM
    {
        $$.ndim = 1;
        $$.dims[0] = $3;
    }
    ;

program : {symbol_table[0].type = ST_PROC; call_block(0);} Block {ecall_block();}PERIOD {emit_all();}
    ;

Block :
    {
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
    }
    ;

if_then: IF CONDITION THEN
    {
        $$.insfalse = GenIns(iJPC, 0, 0);
    }
    ;
Stmt: conditional_stmt
    |
    other_stmt
    ;

conditional_stmt: FOR LPAR Stmt SEMICOLON
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
    }
    |if_then Stmt ELSE
    <bp>{
        $$.ins1 = GenIns(iJMP, 0, 0);
        assert(instructs[$1.insfalse].iid == iJPC);
        backpatch($1.insfalse);
    }
    Stmt
    {
        assert(instructs[$4.ins1].iid == iJMP);
        backpatch($4.ins1);
    }
    |if_then Stmt
    {
        assert(instructs[$1.insfalse].iid == iJPC);
        backpatch($1.insfalse);

    } %prec LOWER_THAN_ELSE
    /* | IF CONDITION error Stmt { yyerror("Missing then"); } */
    |
    <bp>{
        $$.ins1 = nInstructs;
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
    }
    ;
other_stmt:IDENT ASSIGN Exp
    {
        int sid = find_symbol($1);
        GenIns(iSTO, nLevel - symbol_table[sid].nLevel, symbol_table[sid].addr);
    }
    | CALL IDENT
    {
        int sid = find_symbol($2);
        GenIns(iCAL, nLevel - symbol_table[sid].nLevel, symbol_table[sid].addr);
    }
    | READ LPAR
    {
        reading = true;
    }
    idents
    {
        reading = false;
    }
    RPAR
    | WRITE LPAR
    {
        writing = true;
    }
    listExp1
    {
        writing = false;
    }
    RPAR
    | BBEGIN listStmt1 END
    |
    ;

ConstDecl : CONST listConstDef1 SEMICOLON
    ;
ConstDef : IDENT EQ INTLITERAL { $$.name = $1; $$.val = $3; }
    ;
VarDecl : { declaringVar = true; }VAR idents_lit SEMICOLON {declaringVar = false;}
    ;

FuncDecl : FuncDecl FuncHead
    {
        call_block(nLevel + 1);
    }
    Block
    {
        ecall_block();
    }
    SEMICOLON
    | FuncHead
    {
        call_block(nLevel + 1);
    }
    Block
    {
        ecall_block();
    }
    SEMICOLON
    ;
FuncHead : PROCEDURE IDENT
    {
        register_proc($2, nLevel);
    }
    SEMICOLON
    ;

CONDITION : Exp RelOp Exp
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
    {
        GenIns(iOPR, 0, 6);
    }
    ;

Exp : Exp ADD Exp
    {
        GenIns(iOPR, 0, 2);
    }
    | Exp SUB Exp
    {
        GenIns(iOPR, 0, 3);
    }
    | Exp MUL Exp
    {
        GenIns(iOPR, 0, 4);
    }
    | Exp DIV Exp
    {
        GenIns(iOPR, 0, 5);
    }
    | SUB Exp %prec NEG
    {
        GenIns(iOPR, 0, 1);
    }
    | LPAR Exp RPAR
    | LPAR Exp error { yyerror("Missing right parenthesis"); }
    | IDENT
    {
        int sid = find_symbol($1);
        if (symbol_table[sid].type == ST_VAR) GenIns(iLOD, nLevel - symbol_table[sid].nLevel, symbol_table[sid].addr);
        else if (symbol_table[sid].type == ST_CONST) GenIns(iLIT, 0, symbol_table[sid].attr.const_attr.val);
    }
    | IDENT indexes
    | INTLITERAL
    {
        GenIns(iLIT, 0, $1);
    }
    ;

RelOp : EQ { $$ = REL_EQ; }
    | NEQ { $$ = REL_NEQ; }
    | LTE { $$ = REL_LTE; }
    | GTE { $$ = REL_GTE; }
    | LT { $$ = REL_LT; }
    | GT { $$ = REL_GT; }
    ;
%%

void yyerror(const char *s)
{
    fprintf(stderr, "%s\n", s);
}

int main(void)
{
    yyparse();
    return 0;
}