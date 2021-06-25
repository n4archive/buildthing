#ifndef TOKEN_STREAM_H
#define TOKEN_STREAM_H
#include "char_stream.h"
enum _token_type {
	NONE
};
typedef enum _token_type token_type;

struct _token {
	token_type type;
	char* raw;
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

char* _tkstr_read_while(token_stream* s, bool (*predicate) (char));
token* _tkstr_read_next(token_stream* s);
token* _tkstr_read_number(token_stream* s);
#endif
