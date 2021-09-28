#include "debug_any.h"
#include "../parser/debug_ast.h"
#include <stdio.h>
void printiany(iany* value) {
	if (value == NULL) {
		printf("NULL\n");
		return;
	}
	switch (value->type) {
		case IBOOL:
			printf("bool %d\n", *((int*)value->value));
			break;
		case INUM:
			printf("int %lf\n", *((double*)value->value));
			break;
		case ISTR:
			printf("str \"%s\"\n", *((char**)value->value));
			break;
		case IAST_PROG:
			printinsr((ast_node*)value->value);
			printf("\n");
			break;
		default:
			printf("Can't handle type\n");
			break;
	}
}
