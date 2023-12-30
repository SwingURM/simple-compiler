#define MAX_INSNAME_LEN 10
enum INSTRUCTid {
  iJMP,
  iJPC,
  iINT,
  iLIT,
  iLOD,
  iSTO,
  iCAL,
  iOPR,
  iEND, // used for iteration
};

struct INSTRUCT {
  enum INSTRUCTid iid;
  int nLevel;
  int op;
};

static const char *id2name[] = {
    [iJMP] = "JMP",         [iJPC] = "JPC",       [iINT] = "INT",
    [iLIT] = "LIT",         [iLOD] = "LOD",       [iSTO] = "STO",
    [iCAL] = "CAL",         [iOPR] = "OPR"
};