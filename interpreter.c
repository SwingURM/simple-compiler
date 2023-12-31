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
void print_stack(int* Stack, int rsp, int rbp) {
  printf("rsp: %d, rbp: %d\n", rsp, rbp);
  printf("Stack: ");
  for (int i = rbp; i < rsp; i++) {
    printf("%d ", Stack[i]);
  }
  printf("\n");
}

//  解释程序
void Interpreter() {
  int pc = 0;
  int Stack[STACK_SIZE];  //  数据存储器,数据栈
  int rsp = 0;            //  栈顶指针
  int rbp = 0;            //  调用链？

  Stack[0] = Stack[1] = Stack[2] = 0;

  struct INSTRUCT* ins = &instructs[pc];
  do {
    ins = &instructs[pc];
    pc++;
    // print_cur_ins(ins);
    // print_stack(Stack, rsp, rbp);
    switch (ins->iid) {
      case iJMP:
        pc = ins->op;
        break;
      case iJPC:
        if (Stack[--rsp] == 0) pc = ins->op;
        break;
      case iLIT:
        Stack[rsp++] = ins->op;
        break;
      case iLOD:
        Stack[rsp++] = Stack[GetBase(ins->nLevel, Stack, rbp) + ins->op];
        break;
      case iSTO:
        Stack[GetBase(ins->nLevel, Stack, rbp) + ins->op] = Stack[--rsp];
        break;
      case iCAL:
        Stack[rsp] = GetBase(ins->nLevel, Stack, rbp);
        Stack[rsp + 1] = rbp;
        Stack[rsp + 2] = pc;
        rbp = rsp;
        pc = ins->op;
        break;
      case iINT:
        rsp += ins->op;
        break;
      case iOPR:
        switch (ins->op) {
          case 0:
            rsp = rbp;
            pc = Stack[rsp + 2];
            rbp = Stack[rsp + 1];
            break;
          case 1:
            Stack[rsp - 1] = -Stack[rsp - 1];
            break;
          case 2:
            rsp--;
            Stack[rsp - 1] = Stack[rsp - 1] + Stack[rsp];
            break;
          case 3:
            rsp--;
            Stack[rsp - 1] = Stack[rsp - 1] - Stack[rsp];
            break;
          case 4:
            rsp--;
            Stack[rsp - 1] = Stack[rsp - 1] * Stack[rsp];
            break;
          case 5:
            rsp--;
            Stack[rsp - 1] = Stack[rsp - 1] / Stack[rsp];
            break;
          case 6:
            Stack[rsp - 1] = Stack[rsp - 1] % 2;
            break;
          case 8:
            rsp--;
            Stack[rsp - 1] = Stack[rsp - 1] == Stack[rsp];
            break;
          case 9:
            rsp--;
            Stack[rsp - 1] = Stack[rsp - 1] != Stack[rsp];
            break;
          case 10:
            rsp--;
            Stack[rsp - 1] = Stack[rsp - 1] < Stack[rsp];
            break;
          case 11:
            rsp--;
            Stack[rsp - 1] = Stack[rsp - 1] >= Stack[rsp];
            break;
          case 12:
            rsp--;
            Stack[rsp - 1] = Stack[rsp - 1] > Stack[rsp];
            break;
          case 13:
            rsp--;
            Stack[rsp - 1] = Stack[rsp - 1] <= Stack[rsp];
            break;
          case 14:
            printf("%d", Stack[--rsp]);
            break;
          case 15:
            printf("\n");
            break;
          case 16:
            scanf("%d", &Stack[rsp++]);
            break;
          default:
            assert(0);
        }
        break;
      default:
        assert(0);
    }
    // print_stack(Stack, rsp, rbp);
  } while (pc != 0);
}

int main(int argc, char** argv) {
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