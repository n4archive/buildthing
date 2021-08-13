#ifndef TOKEN_STREAM_H
#define TOKEN_STREAM_H
#include "char_stream.h"
enum _token_type {
	NUMBER = 1, VAR = 2, KEYWORD = 3, STRING = 4, PUNC = 5, OP = 6
};
typedef enum _token_type token_type;

struct _token {
	token_type type;
	char* raw;
	double numberValue;
};
typedef struct _token token;

struct _token_stream_struct {
	input_stream* instr;
	token* current;
	bool failed;
	bool eof;
};
typedef struct _token_stream_struct token_stream;

token_stream* new_tk_stream(input_stream* input);
token* tkstr_next(token_stream* s);
token* tkstr_peek(token_stream* s);
void tkstr_fail(token_stream* s, char* fail);
//In case you want to keep the input stream, set the internal pointer (instr) to null before calling
void tkstr_free(token_stream* s);
//In case you want to keep the raw string, use free(t)
void destroy_token(token* t);

bool is_keyword(char* s);
char* _tkstr_read_while(token_stream* s, bool (*predicate) (char));
token* _tkstr_read_next(token_stream* s);
token* _tkstr_read_number(token_stream* s);
token* _tkstr_read_ident(token_stream* s);
char* _tkstr_read_escaped(token_stream* s, char end);
token* _tkstr_read_string(token_stream* s);
void _tkstr_skip_comment(token_stream* s);
#endif
