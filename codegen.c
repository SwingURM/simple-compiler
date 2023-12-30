#include "codegen.h"
#include "symbol.h"

struct INSTRUCT instructs[MAX_NUMBER_INSTRUCTS];
int nInstructs = 0;


int GenIns(int iid, int nLevel, int op) {
    int retVal = nInstructs;
    assert(nInstructs < MAX_NUMBER_INSTRUCTS);
    instructs[nInstructs].iid = iid;
    instructs[nInstructs].nLevel = nLevel;
    instructs[nInstructs].op = op;
    nInstructs++;
    return retVal;
}


void emit_all() {
    // table_printer();
    // printf("-----------------------\n");
    for (int i = 0; i < nInstructs; i++) {
        printf("%s %d %d\n", id2name[instructs[i].iid], instructs[i].nLevel, instructs[i].op);
    }
}