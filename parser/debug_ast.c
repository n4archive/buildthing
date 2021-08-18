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
    printf("{\"type\": \"num\", \"value\": %lf}", j->num);
    b;
  case AST_VAR:
    a(k);
    printf("{\"type\": \"var\", \"value\": \"%s\"}", k->str);
    b;
  case AST_UNARY:
    a(l);
    printf("{\"type\": \"unary\", \"operator\": \"%s\", \"body\": ",
           l->operatorr);
    printinsr(l->contents);
    printf("}");
    b;
  case AST_PROG:
    a(m);
    printf("{\"type\": \"prog\", \"body\": [");
    for (int i2 = 0; i2 < m->count; i2++) {
      printinsr(m->instrs[i2]);
      if (i2 + 1 < m->count)
        printf(",");
    }
    printf("]}");
    b;
  case AST_IF:
    a(n);
    char *d = "error";
    switch (n->type) {
    case AST_DOWHILE_OP:
      d = "dowhile";
      b;
    case AST_IF_OP:
      d = "if";
      b;
    case AST_WHILE_OP:
      d = "while";
      b;
    }
    printf("{\"type\": \"if\", \"type\"=\"%s\", \"cond\":", d);
    printinsr(n->cond);
    printf(", \"then\"=");
    printinsr(n->then);
    printf(" \"else\"=");
    if (n->elsee)
      printinsr(n->elsee);
    else
      printf("null");
    printf("}");
    b;
  case AST_FUNC:
    a(o);
    printf("{\"type\": \"func\", \"arg\": [");
    for (int i2 = 0; i2 < o->argc; i2++) {
      printinsr(o->argv[i2]);
      if (i2 + 1 < o->argc)
        printf(",");
    }
    printf("], \"body\"=");
    printinsr(o->body);
    printf("}");
    b;
  case AST_CALL:
    a(p);
    printf("{\"type\": \"call\", \"arg\": [");
    for (int i2 = 0; i2 < p->argc; i2++) {
      printinsr(p->argv[i2]);
      if (i2 + 1 < p->argc)
        printf(",");
    }
    printf("], \"ref\"=");
    printinsr(p->ref);
    printf("}");
    b;
  case AST_BOOL:
    a(q);
    printf("{\"type\": \"bool\", \"value\": %d}", q->value);
    b;
  case AST_BINARY:
    a(r);
    printf("{\"type\": \"binary\", \"left\": ");
    printinsr(r->left);
    printf(", \"operator\": \"%s\", \"right\": ", r->operatorr);
    printinsr(r->right);
    printf("}");
    b;
  case AST_ASSIGN:
    a(s);
    printf("{\"type\": \"assign\", \"left\": ");
    printinsr(s->left);
    printf(", \"right\": ");
    printinsr(s->right);
    printf("}");
    b;
  case AST_STR:
    t = i->content;
    printf("{\"type\": \"string\", \"value\": \"%s\"", t->str);
    b;
  }
}
#undef a
#undef b
