#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "char_stream.h"

input_stream* new_in_stream(char* input) {
	input_stream* s = malloc(sizeof(input_stream));
	s->_data = input;
	s->pos = 0;
	s->line = 0;
	s->col = 0;
	s->failed = false;
	s->eof = false;
	s->len = strlen(input);
	return s;
}

char instr_next(input_stream* s) {
	if (s->eof)
		return '\0';
	char c = s->_data[s->pos++];
	if (c == '\n') {
		s->line++;
		s->col = 0;
	} else if (c == '\0') {
		s->eof = true;
	} else {
		s->col++;
	}
	return c;
}

char instr_peek(input_stream* s) {
	if (s->eof)
		return '\0';
	else
		return s->_data[s->pos];
}

void instr_fail(input_stream* s, char* fail) {
	s->failed = true;
	s->eof = true;
	printf("error at %ld:%ld: %s\n", s->line, s->col, fail);
}

//In case you want to keep the data, just run free(s)
void instr_free(input_stream* s) {
	free(s->_data);
	free(s);
}
