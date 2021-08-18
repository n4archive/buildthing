#include "debug_ast.h"
#include <stdio.h>
#define a(c) c = i->content
#define b break
void printinsr(ast_node *i) {
  switch (i->type) {
    ast_number *j;
    ast_var *k;
    ast_unary *l;
    ast_prog *m;
    ast_if *n;
    ast_func *o;
    ast_call *p;
    ast_bool *q;
    ast_binary *r;
    ast_assign *s;
    ast_str *t;
  case AST_NUM:
    a(j);
    printf("{num %lf}\n", j->num);
    b;
  case AST_VAR:
    a(k);
    printf("{var %s}", k->str);
    b;
  case AST_UNARY:
    a(l);
    printf("{unary %s \n", l->operatorr);
    printinsr(l->contents);
    printf("}\n");
    b;
  case AST_PROG:
    a(m);
    printf("{prog [\n");
    for (int i2 = 0; i2 < m->count; i2++) {
      printinsr(m->instrs[i2]);
      if (i2 + 1 < m->count)
        printf(",\n");
    }
    printf("]}\n");
    b;
  case AST_IF:
    a(n);
    printf("{if type=%d cond=\n", n->type);
    printinsr(n->cond);
    printf(" then=\n");
    printinsr(n->then);
    printf(" elsee=\n");
    if (n->elsee)
      printinsr(n->elsee);
    printf("}\n");
    b;
  case AST_FUNC:
    a(o);
    printf("{func arg=[");
    for (int i2 = 0; i2 < o->argc; i2++) {
      printinsr(o->argv[i2]);
      if (i2 + 1 < o->argc)
        printf(",\n");
    }
    printf("], body=\n");
    printinsr(o->body);
    printf("}\n");
    b;
  case AST_CALL:
    a(p);
    printf("{call arg=[");
    for (int i2 = 0; i2 < p->argc; i2++) {
      printinsr(p->argv[i2]);
      if (i2 + 1 < p->argc)
        printf(",\n");
    }
    printf("], ref=\n");
    printinsr(p->ref);
    printf("}\n");
    b;
  case AST_BOOL:
    a(q);
    printf("{bool %d}\n", q->value);
    b;
  case AST_BINARY:
    a(r);
    printf("{binary left=\n");
    printinsr(r->left);
    printf(" operatorr=%s right=\n", r->operatorr);
    printinsr(r->right);
    printf("}\n");
    b;
  case AST_ASSIGN:
    a(s);
    printf("{assign left=\n");
    printinsr(s->left);
    printf(" right=\n");
    printinsr(s->right);
    printf("}\n");
    b;
  case AST_STR:
    t = i->content;
    printf("\"%s\"", t->str);
    b;
  }
}
#undef a
#undef b
