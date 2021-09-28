#ifndef INTERPRETER_H
#define INTERPRETER_H
#include "../parser/parser.h"
enum itype {
  INUM,
  ISTR,
  IBOOL,
  IAST_PROG
};
typedef struct {
	enum itype type;
	void* value;
} iany;
typedef struct {
	char* key;
	iany* value;
} __ienv_varMap;
typedef struct __ienv {
	void* varMap;
	jmp_buf* exception;
	struct __ienv* parent;
} ienv;
int __ienv_compar(const void *l, const void *r);
void _ienv_put(ienv* e, char* key, iany* value);
void _ienv_set(ienv* e, char* key, iany* value);
iany* _ienv_get(ienv* e, char* key);
ienv* _ienv_new(ienv* parent); /* parent can not be null, if that is wanted, use alloc */
ienv* _ienv_alloc(jmp_buf* exception);
iany* evaluate(ast_node* prog, jmp_buf* exception);
iany* subeval_prog(ienv* e, ast_node* prog);
#endif
