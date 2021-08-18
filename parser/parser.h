#ifndef PARSER_H
#define PARSER_H
#include "token_stream.h"
#include <stdbool.h>

enum ast_type {
  AST_NUM,
  AST_STR,
  AST_BOOL,
  AST_VAR,
  AST_CALL,
  AST_IF,
  AST_ASSIGN,
  AST_BINARY,
  AST_FUNC,
  AST_PROG,
  AST_UNARY
};
enum if_type { IF_OP, DOWHILE_OP, WHILE_OP };

typedef struct {
  enum ast_type type;
  // type of content depends on the node type
  void *content;
} ast_node;

typedef struct {
  double num;
} ast_number;
typedef struct {
  char *str;
} ast_str;
typedef struct {
  // abusing char because small
  char value;
} ast_bool;
// the same in the end xD
typedef ast_str ast_var;
typedef struct {
  ast_node *ref;
  ast_node **argv;
  int argc;
} ast_call;
typedef struct {
  enum if_type type;
  ast_node *cond;
  ast_node *then;
  ast_node *elsee;
} ast_if;
typedef struct {
  ast_node *left;
  ast_node *right;
} ast_assign;
typedef struct {
  char *operatorr;
  ast_node *left;
  ast_node *right;
} ast_binary;
typedef struct {
  ast_node **instrs;
} ast_prog;
typedef struct {
  ast_node *contents;
  char *operatorr;
} ast_unary;
typedef struct {
  ast_prog *body;
  ast_var **argv;
  int argc;
} ast_func;

char *ef(int);

#define parsef(i) ast_node *parse_##i(token_stream *input);
parsef(func) parsef(bool) parsef(expression) parsef(varname) parsef(if)
    parsef(prog) parsef(atom);
void skip_punc(token_stream *input, char punc);
bool ensure_punc(token_stream *input, char punc);
int parse_delimited(token_stream *input, char start, char stop, char delimiter,
                    ast_node ***ret, ast_node *(*parser)());
#undef parsef
#endif
