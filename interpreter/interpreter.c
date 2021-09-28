#include "interpreter.h"
#include "../unused.h"
#include <search.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char ierr[500];
char ierrctx[200];
char *ie(int UNUSED(i)) {
  MARK_UNUSED(i);
  return ierr;
}

int __ienv_compar(const void *l, const void *r) {
	const __ienv_varMap* ml = l;
	const __ienv_varMap* mr = r;
	return strcmp(ml->key, mr->key);
}
void _ienv_fail(ienv* e, char* err) {
	printf("%s", err);
	longjmp(*(e->exception), 1);
}
void _ienv_put(ienv* e, char* key, iany* value) {
	__ienv_varMap* m = malloc(sizeof(__ienv_varMap));
	m->key = key;
	if (tfind(m, &e->varMap, __ienv_compar)) {
		free(m);
		_ienv_fail(e, ie(sprintf(ierr, "double definition of variable \"%s\"", key)));
	}
	m->value = value;
	tsearch(m, &e->varMap, __ienv_compar); //insert

}
void _ienv_set(ienv* e, char* key, iany* value) {
	__ienv_varMap* m = malloc(sizeof(__ienv_varMap));
	ienv* e2 = e;
	m->key = key;
	__ienv_varMap* r = tfind(m, &e->varMap, __ienv_compar);
	while (!r) {
		e2 = e2->parent;
		if (!e2)
			break;
		r = tfind(m, &e2->varMap, __ienv_compar);
	}
	if (!r) {
		free(m);
		_ienv_fail(e, ie(sprintf(ierr, "tried to assign to undefined variable \"%s\"", key)));
	}
	(*(__ienv_varMap**)r)->value = value;
}
iany* _ienv_get(ienv* e, char* key) {
	__ienv_varMap* m = malloc(sizeof(__ienv_varMap));
	ienv* e2 = e;
	m->key = key;
	__ienv_varMap* r = tfind(m, &e->varMap, __ienv_compar);
	while (!r) {
		e2 = e2->parent;
		if (!e2)
			break;
		r = tfind(m, &e2->varMap, __ienv_compar);
	}
	if (!r) {
		free(m);
		_ienv_fail(e, ie(sprintf(ierr, "tried to access undefined variable \"%s\"", key)));
	}
	return (*(__ienv_varMap**)r)->value;
}
ienv* _ienv_new(ienv* parent) { /* parent can not be null, if that is wanted, use alloc */
	ienv* a = _ienv_alloc(parent->exception);
	a->parent = parent;
	return a;
}
ienv* _ienv_alloc(jmp_buf* exception) {
	ienv* a = malloc(sizeof(ienv));
	a->exception = exception;
	a->parent = NULL;
	a->varMap = NULL;
	return a;
}
iany* evaluate(ast_node* prog, jmp_buf* exception) {
	ienv* root = _ienv_alloc(exception);
	return subeval_prog(root, prog);
}
iany* subeval_prog(ienv* e, ast_node* prog) {
	//todo
	return NULL;
}
