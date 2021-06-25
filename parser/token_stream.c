#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "token_stream.h"

token_stream* new_tk_stream(input_stream* input) {
	token_stream* s = malloc(sizeof(token_stream));
	s->instr = input;
	s->failed = false;
	s->eof = false;
	return s;
}

void sync_tkstr_fail(token_stream* s) {
	if (s->instr->failed)
		s->failed = true;
	if (s->instr->eof)
		s->eof = true;
}

token* tkstr_next(token_stream* s) {
	sync_tkstr_fail(s);
	if (s->eof)
		return NULL;
	token* t = s->current;
	s->current = NULL;
	if (t)
		return t;
	else
		return _tkstr_read_next(s);
}

token* tkstr_peek(token_stream* s) {
	sync_tkstr_fail(s);
	if (s->eof)
		return NULL;
	else if (s->current)
		return s->current;
	else
		return (s->current = _tkstr_read_next(s));
}

void tkstr_fail(token_stream* s, char* fail) {
	s->failed = true;
	instr_fail(s->instr, fail);
}

token* _tkstr_read_next(token_stream* s) {
	sync_tkstr_fail(s);
	if (s->eof)
		return NULL;
	return NULL; //TODO
}
