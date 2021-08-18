#include "file.h"
#include "parser/parser.h"
#include "parser/token_stream.h"
#include "string.h"
#include <setjmp.h>
#include <stdio.h>

bool p(char c) { return c != 'G'; }

int main(int argc, char **argv) {
  if (argc != 2)
    return 1;
  char *contents = readfullfile(argv[1]);
  if (contents == NULL)
    return 2;
  jmp_buf env;
  int eret;
  if ((eret = setjmp(env)) == 0) {
    token_stream *s = new_tk_stream(new_in_stream(contents, &env));
    /*while (!s->eof) {
      token *n = tkstr_next(s);
      if (n != NULL) {
        printf("Type: %d | Content: %s | Numeric value: %f\n", n->type, n->raw,
               n->numberValue);
        destroy_token(n);
      }
    }*/
    parse_prog(s);
    tkstr_free(s);
  } else {
    printf("ERROR, magic num %d, bye bye", eret);
  }
  return 0;
}
