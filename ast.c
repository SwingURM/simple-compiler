#include "ast.h"
tree root;
#define STACK_SIZE 100
tree ast_stack[STACK_SIZE];
static int nStack = 0;
void print_tree(tree, int);
tree add_child(tree, int);
static void push(tree t) {
    ast_stack[nStack++] = t;
}

static void pop(){
    nStack--;
}

static tree stacktop() {
    return ast_stack[nStack - 1];
}

void add_ast(int type) {
    tree parent = stacktop();
    tree child = add_child(parent, type);
    push(child);
    leave_ast();
}

void enter_ast(int type) {
    tree parent = stacktop();
    tree child = add_child(parent, type);
    push(child);
}

void leave_ast() {
    pop();
}

tree new_tree(int type) {
    tree t = (tree)malloc(sizeof(struct tree_struct));
    t->val.type = type;
    t->children = malloc(sizeof(struct list_struct));
    t->children->node = NULL;
    t->children->next = NULL;
    return t;
}

void init_tree() {
    root = new_tree(nPROGRAM);
    push(root);
}

tree add_child(tree parent, int type) {
    assert(parent);
    assert(parent->children);
    // allocate memory for child
    tree child = new_tree(type);
    // add child to parent's children list
    list l = malloc(sizeof(struct list_struct));
    l->node = child;
    l->next = parent->children->next;
    parent->children->next = l;
    return child;
}

void print_ast() {
    print_tree(root, 0);
}

void print_tree(tree t, int depth) {
    if (!t) return;
    for (int i = 0; i < depth; i++) printf("  ");
    printf("%s\n", node_type_str[t->val.type]);
    list l = t->children->next;
    while (l) {
        print_tree(l->node, depth + 1);
        l = l->next;
    }
}

void free_tree(tree t) {
    if (!t) return;
    list l = t->children->next;
    while (l) {
        free_tree(l->node);
        l = l->next;
    }
    free(t->children);
    free(t);
}

// int main() {
//     // try to build a tree
//     init_tree();
//     tree t = root;
//     add(nCONST_DECL);
//     leave();
//     add(nVAR_DECL);
//     add(nCONST_DECL);
//     add(nPROGRAM);
//     print_tree(root, 0);
//     free_tree(root);
// }