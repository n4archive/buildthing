#include "parser.h"
#include "regex.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BOX(typeee, contenttt)                                                 \
  ast_node *rret = malloc(sizeof(ast_node));                                   \
  rret->content = contenttt;                                                   \
  rret->type = typeee;                                                         \
  return rret;

char err[500];
inline char *ef(int i) { return err; }

void skip_punc(token_stream *input, char punc) {
  token *t = tkstr_next(input);
  if (!(t->type == PUNC && t->raw[0] == punc && t->raw[1] == '\0'))
    tkstr_fail(input, ef(sprintf(err, "Unexcepted input, I wanted %c", punc)));
  destroy_token(t);
}

bool ensure_punc(token_stream *input, char punc) {
  token *t = tkstr_peek(input);
  return t->type == PUNC && t->raw[0] == punc && t->raw[1] == '\0';
}

void skip_op(token_stream *input, char *op) {
  token *t = tkstr_next(input);
  if (!(t->type == OP && strcmp(t->raw, op) == 0))
    tkstr_fail(input, ef(sprintf(err, "Unexcepted input, I wanted %s", op)));
  destroy_token(t);
}

bool ensure_op(token_stream *input, char *op) {
  token *t = tkstr_peek(input);
  return t->type == OP && strcmp(op, t->raw) == 0;
}

bool ensure_kw(token_stream *input, char *kw) {
  token *t = tkstr_peek(input);
  return t->type == KEYWORD && strcmp(kw, t->raw) == 0;
}

void skip_kw(token_stream *input, char *kw) {
  token *t = tkstr_next(input);
  if (!(strcmp(kw, t->raw) == 0 && t->type == KEYWORD))
    tkstr_fail(input, ef(sprintf(err, "Unexcepted input, I wanted %s", kw)));
  destroy_token(t);
}

int parse_delimited(token_stream *input, char start, char stop, char delimiter,
                    ast_node ***ret, ast_node *(*parser)(token_stream *)) {
  int size = 5;
  ast_node **a = malloc(sizeof(ast_node *) * size);
  int count = 0;
  bool first = true;
  skip_punc(input, start);
  while (!input->eof) {
    if (ensure_punc(input, stop))
      break;
    if (first)
      first = false;
    else
      skip_punc(input, delimiter);
    if (ensure_punc(input, stop))
      break; // the last separator can be missing
    a[count++] = parser(input);
    if (count == size) {
      size += 5;
      a = realloc(a, size * sizeof(ast_node *));
    }
  }
  skip_punc(input, stop);
  *ret = a;
  return count;
}

ast_node *parse_varname(token_stream *input) {
  token *t = tkstr_next(input);
  if (t->type != VAR)
    tkstr_fail(input, "Excepted variable name");
  ast_var *tret = malloc(sizeof(ast_var));
  tret->str = t->raw;
  free(t);
  BOX(AST_VAR, tret)
}

int prec(char *op) {
#define c(a, b)                                                                \
  if (strcmp(op, a) == 0)                                                      \
    return b;
  c("=", 1) c("||", 2) c("&&", 3) c("<", 7) c(">", 7) c("<=", 7) c(">=", 7)
      c("==", 7) c("!=", 7) c("+", 10) c("-", 10) c("*", 20) c("/", 20)
          c("%", 20) return -1;
#undef c
}

ast_node *parse_bool(token_stream *input) {
  token *t = tkstr_next(input);
  if (t->type != KEYWORD)
    tkstr_fail(input, "Unexcepted");
  ast_bool *ret = malloc(sizeof(ast_bool));
  ret->value = strcmp(t->raw, "true") == 0;
  BOX(AST_BOOL, ret)
}

ast_node *parse_if(token_stream *input) {
  skip_kw(input, "if");
  ast_node *cond = parse_expression(input);
  if (!ensure_punc(input, '{'))
    skip_kw(input, "then");
  ast_node *then = parse_expression(input);
  ast_if *ret = malloc(sizeof(ast_if));
  if (ensure_kw(input, "else")) {
    destroy_token(tkstr_next(input));
    ret->elsee = parse_expression(input);
  }
  ret->cond = cond;
  ret->then = then;
  BOX(AST_IF, ret)
}

ast_node *parse_call(token_stream *input, ast_node *name) {
  ast_call *ret = malloc(sizeof(ast_call));
  ast_node **tempptr;
  ret->ref = name;
  ret->argc = parse_delimited(input, '(', ')', ',', &tempptr, parse_varname);
  ret->argv = malloc(sizeof(ast_var) * ret->argc);
  for (int i = 0; i < ret->argc; i++) {
    ret->argv[i] = tempptr[i]->content;
    free(tempptr[i]);
  }
  BOX(AST_CALL, ret)
}

ast_node *maybe_call(token_stream *input, ast_node *(*func)(token_stream *)) {
  ast_node *expr = func(input);
  if (ensure_punc(input, '('))
    return parse_call(input, expr);
  return expr;
}

ast_node *maybe_binary(token_stream *input, ast_node *left, int my_prec) {
  token *tok = tkstr_peek(input);
  if (tok->type == OP) {
    int his_prec = prec(tok->raw);
    if (his_prec > my_prec) {
      bool is_assign = ensure_op(input, "=");
      destroy_token(tkstr_next(input)); // consume the token we peeked
      ast_node *rett = malloc(sizeof(ast_node));
      ast_node *right = maybe_binary(input, parse_atom(input), his_prec);
      if (is_assign) {
        rett->type = AST_ASSIGN;
        ast_assign *ret = malloc(sizeof(ast_assign));
        ret->left = left;
        ret->right = right;
        rett->content = ret;
      } else {
        rett->type = AST_BINARY;
        ast_binary *ret = malloc(sizeof(ast_binary));
        ret->operatorr = tok->raw;
        ret->left = left;
        ret->right = right;
        rett->content = ret;
      }
      return maybe_binary(input, rett, my_prec);
    }
  }
  return left;
}

ast_node *_real_parse_atom(token_stream *input) {
  if (ensure_punc(input, '(')) {
    destroy_token(tkstr_next(input));
    ast_node *exp = parse_expression(input);
    skip_punc(input, ')');
    return exp;
  }

  if (ensure_op(input, "!")) {
    skip_op(input, "!");
    ast_unary *ret = malloc(sizeof(ast_unary));
    ret->operatorr = "!";
    ret->contents = parse_atom(input);
    BOX(AST_UNARY, ret)
  }

  if (ensure_punc(input, '{'))
    return parse_prog(input);
  if (ensure_kw(input, "if"))
    return parse_if(input);
  if (ensure_kw(input, "true") || ensure_kw(input, "false"))
    return parse_bool(input);
  if (ensure_kw(input, "function")) {
    destroy_token(tkstr_next(input));
    return parse_func(input);
  }
  token *tok = tkstr_next(input);
  if (tok->type == VAR) {
    ast_var *ret = malloc(sizeof(ast_var));
    ret->str = tok->raw;
    BOX(AST_VAR, ret)
  }
  if (tok->type == STRING) {
    ast_str *ret = malloc(sizeof(ast_str));
    ret->str = tok->raw;
    BOX(AST_STR, ret)
  }
  if (tok->type == NUMBER) {
    ast_number *ret = malloc(sizeof(ast_number));
    ret->num = tok->numberValue;
    BOX(AST_NUM, ret)
  }
  tkstr_fail(input, "Unexcepted token");
  return NULL;
}

ast_node *_real_parse_expression(token_stream *input) {
  return maybe_binary(input, parse_atom(input), 0);
}

ast_node *parse_atom(token_stream *input) {
  return maybe_call(input, _real_parse_atom);
}

ast_node *parse_expression(token_stream *input) {
  return maybe_call(input, _real_parse_expression);
}

ast_node *parse_func(token_stream *input) {
  ast_func *ret = malloc(sizeof(ast_func));
  ast_node **tempptr;
  ret->argc = parse_delimited(input, '(', ')', ',', &tempptr, parse_varname);
  ret->argv = malloc(sizeof(ast_var) * ret->argc);
  for (int i = 0; i < ret->argc; i++) {
    ret->argv[i] = tempptr[i]->content;
    free(tempptr[i]);
  }
  BOX(AST_FUNC, ret)
}

ast_node *parse_prog(token_stream *input) {
  int size = 5;
  ast_node **prog = malloc(sizeof(ast_node *) * size);
  int count = 0;
  while (!input->eof) {
    prog[count++] = parse_expression(input);
    if (!input->eof)
      skip_punc(input, ';');
  }
  if (count == size) {
    size += 5;
    prog = realloc(prog, size * sizeof(ast_prog *));
  }
  ast_prog *ret = malloc(sizeof(ast_prog));
  ret->instrs = prog;
  BOX(AST_PROG, ret)
}
