#ifndef REGEX_H
#define REGEX_H
#define MAKE_CHAR_FUNC_H(name) int name(char c);

MAKE_CHAR_FUNC_H(is_id_start)
MAKE_CHAR_FUNC_H(is_id)
MAKE_CHAR_FUNC_H(is_digit)
MAKE_CHAR_FUNC_H(is_op_char)
MAKE_CHAR_FUNC_H(is_punc)
MAKE_CHAR_FUNC_H(is_whitespace)
#endif
