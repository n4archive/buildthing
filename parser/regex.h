#ifndef REGEX_H
#define REGEX_H
#define MAKE_CHAR_FUNC_H(name) bool name(char c);
#include <stdbool.h>

// DEPRECATED
char *char2string(char c);

/* normal functions */
bool is_keyword(char *);
MAKE_CHAR_FUNC_H(is_not_newline)

/* regex functions */
MAKE_CHAR_FUNC_H(is_id_start)
MAKE_CHAR_FUNC_H(is_id)
MAKE_CHAR_FUNC_H(is_digit)
MAKE_CHAR_FUNC_H(is_hex_digit)
MAKE_CHAR_FUNC_H(is_op_char)
MAKE_CHAR_FUNC_H(is_punc)
MAKE_CHAR_FUNC_H(is_whitespace)
#endif
