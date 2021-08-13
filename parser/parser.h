#ifndef PARSER_H
#define PARSER_H

enum ast_type { NUM, STR, BOOL, VAR, CALL, IF, ASSIGN, BINARY, FUNC };

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

#endif
