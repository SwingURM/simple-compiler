#include "interpreter.h"

struct INSTRUCT instructs[MAX_NINSTRUCTS];
int nins = 0;

int GetBase(int nNum, int* pStack, int nStackCurrentBase) {
  int nStackIndex;

  nStackIndex = nStackCurrentBase;
  while (nNum > 0) {
    nStackIndex = pStack[nStackIndex];
    nNum--;
  }
  return nStackIndex;
}

struct INSTRUCT* parse_line(FILE* fp) {
  static struct INSTRUCT ins;
  char buf[MAX_INSNAME_LEN + 1];
  fscanf(fp, "%s %d %d", buf, &ins.nLevel, &ins.op);
  for (int i = 0; i < iEND; i++) {
    if (strcmp(buf, id2name[i]) == 0) {
      ins.iid = i;
      break;
    }
  }
  return &ins;
}
void print_cur_ins(struct INSTRUCT* ins) {
  printf("%s %d %d\n", id2name[ins->iid], ins->nLevel, ins->op);
}
void print_stack(int* Stack, int nStackTop, int nStackBase) {
    printf("nStackTop: %d, nStackBase: %d\n", nStackTop, nStackBase);
  printf("Stack: ");
  for (int i = nStackBase; i < nStackTop; i++) {
    printf("%d ", Stack[i]);
  }
  printf("\n");
}

//  解释程序
void Interpreter() {
  //
  int pc = 0;
  int nStackBase = 0;
  int nStackTop = 0;
  int Stack[STACK_SIZE];  //  数据存储器,数据栈

  int rsp = 0;  //  栈顶指针
  int rbp = 0;  //  栈底指针

  Stack[0] = Stack[1] = Stack[2] = 0;

  struct INSTRUCT* ins = &instructs[pc];
  do {
    ins = &instructs[pc];
    pc++;
    // print_cur_ins(ins);
    // print_stack(Stack, nStackTop, nStackBase);
    switch (ins->iid) {
      case iJMP:
        pc = ins->op;
        break;
      case iJPC:
        if (Stack[--nStackTop] == 0) pc = ins->op;
        break;
      case iLIT:
        Stack[nStackTop++] = ins->op;
        break;
      case iLOD:
        Stack[nStackTop++] =
            Stack[GetBase(ins->nLevel, Stack, nStackBase) + ins->op];
        break;
      case iSTO:
        Stack[GetBase(ins->nLevel, Stack, nStackBase) + ins->op] =
            Stack[--nStackTop];
        break;
      case iCAL:
        Stack[nStackTop] = GetBase(ins->nLevel, Stack, nStackBase);
        Stack[nStackTop + 1] = nStackBase;
        Stack[nStackTop + 2] = pc;
        nStackBase = nStackTop;
        pc = ins->op;
        break;
      case iINT:
        nStackTop += ins->op;
        break;
      case iOPR:
        switch (ins->op) {
          case 0:
            nStackTop = nStackBase;
            pc = Stack[nStackTop + 2];
            nStackBase = Stack[nStackTop + 1];
            break;
          case 1:
            Stack[nStackTop - 1] = -Stack[nStackTop - 1];
            break;
          case 2:
            nStackTop--;
            Stack[nStackTop - 1] = Stack[nStackTop - 1] + Stack[nStackTop];
            break;
          case 3:
            nStackTop--;
            Stack[nStackTop - 1] = Stack[nStackTop - 1] - Stack[nStackTop];
            break;
          case 4:
            nStackTop--;
            Stack[nStackTop - 1] = Stack[nStackTop - 1] * Stack[nStackTop];
            break;
          case 5:
            nStackTop--;
            Stack[nStackTop - 1] = Stack[nStackTop - 1] / Stack[nStackTop];
            break;
          case 6:
            Stack[nStackTop - 1] = Stack[nStackTop - 1] % 2;
            break;
          case 8:
            nStackTop--;
            Stack[nStackTop - 1] = Stack[nStackTop - 1] == Stack[nStackTop];
            break;
          case 9:
            nStackTop--;
            Stack[nStackTop - 1] = Stack[nStackTop - 1] != Stack[nStackTop];
            break;
          case 10:
            nStackTop--;
            Stack[nStackTop - 1] = Stack[nStackTop - 1] < Stack[nStackTop];
            break;
          case 11:
            nStackTop--;
            Stack[nStackTop - 1] = Stack[nStackTop - 1] >= Stack[nStackTop];
            break;
          case 12:
            nStackTop--;
            Stack[nStackTop - 1] = Stack[nStackTop - 1] > Stack[nStackTop];
            break;
          case 13:
            nStackTop--;
            Stack[nStackTop - 1] = Stack[nStackTop - 1] <= Stack[nStackTop];
            break;
          case 14:
            printf("%d", Stack[--nStackTop]);
            break;
          case 15:
            printf("\n");
            break;
          case 16:
            scanf("%d", &Stack[nStackTop++]);
            break;
          default:
            assert(0);
        }
        break;
      default:
        assert(0);
    }
    // print_stack(Stack, nStackTop, nStackBase);
  } while (pc != 0);
}

int main(int argc, char** argv) {
  // read instructs from file output.txt

  // read the first arg
  if (argc != 2) {
    printf("Usage: ./interpreter <filename>\n");
    return 0;
  }
  FILE* fp = fopen(argv[1], "r");
  assert(fp != NULL);

  while (!feof(fp)) {
    struct INSTRUCT* ins = parse_line(fp);
    instructs[nins++] = *ins;
  }
  fclose(fp);
  Interpreter();
}