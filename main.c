#include "file.h"
#include "interpreter/interpreter.h"
#include "parser/debug_ast.h"
#include "interpreter/debug_any.h"
#include "string.h"
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  if (argc != 2)
    return 1;
  /* read the file */
  char *contents = readfullfile(argv[1]);
  if (contents == NULL)
    return 2;
  /* set up longjump for error handling */
  jmp_buf env;
  int eret;
  if ((eret = setjmp(env)) == 0) {
    /* main code */
    token_stream *s = new_tk_stream(new_in_stream(contents, &env));
    ast_node *n = parse_prog(s);
    tkstr_free(s);
    printinsr(n); printf("\n");
    iany* ret = evaluate(n, &env);
    printiany(ret);
  } else {
    /* error handling could go here */
    return eret;
  }
  return 0;
}
