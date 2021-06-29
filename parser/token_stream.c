#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "token_stream.h"
#include "regex.h"

token_stream* new_tk_stream(input_stream* input) {
	token_stream* s = malloc(sizeof(token_stream));
	s->instr = input;
	s->current = NULL;
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

char* _tkstr_read_while(token_stream* s, bool (*predicate) (char)) {
	int size = 1000;
	int free = size;
	char* final = malloc(sizeof(char) * size);
	while (!s->eof && !s->failed && predicate(instr_peek(s->instr))) {
		sync_tkstr_fail(s);
		final[size - (free--)] = instr_next(s->instr);
		if (free < 1) {
			size += 100;
			final = realloc(final, size * sizeof(char));
		}
	}
	final[size - free] = '\0';
	return final;
}

char* _tkstr_read_escaped(token_stream* s, char end) {
	int size = 1000;
	int free = size;
	bool escaped = false;
	char* final = malloc(sizeof(char) * size);
	char c = '\0';
	while (!s->eof && !s->failed) {
		c = instr_next(s->instr);
		sync_tkstr_fail(s);
		if (escaped && c == 'a') {
			final[size - (free--)] = '\a';
		} else if (escaped && c == 'b') {
			final[size - (free--)] = '\b';
		} else if (escaped && c == 'f') {
			final[size - (free--)] = '\f';
		} else if (escaped && c == 'n') {
			final[size - (free--)] = '\n';
		} else if (escaped && c == 'r') {
			final[size - (free--)] = '\r';
		} else if (escaped && c == 't') {
			final[size - (free--)] = '\t';
		} else if (escaped && c == 'v') {
			final[size - (free--)] = '\v';
		} else if (escaped) {
			final[size - (free--)] = c;
			escaped = false;
		} else if (c == '\\') {
			escaped = true;
		} else if (c == end) {
			break;
		} else {
			final[size - (free--)] = c;
		}
		if (free < 1) {
			size += 100;
			final = realloc(final, size * sizeof(char));
		}
	}
	final[size - free] = '\0';
	return final;
}

token* _tkstr_read_number(token_stream* s) {
	bool has_dot = false;
	int hexflag = 0;
	bool predread(char c) {
		if (c == '.') {
			if (has_dot) return false;
			has_dot = true;
			return true;
		} else if (c == '0' && hexflag == 0) {
			hexflag = 1;
			return true;
		} else if (hexflag == 0) {
			hexflag = -1;
		} else if (hexflag == 1 && c == 'x') {
			hexflag = 2;
			return true;
		} else if (hexflag == 2) {
			return is_hex_digit(c);
		}
		return is_digit(c);
	}
	char* numStr = _tkstr_read_while(s, predread);
	double number;
	token* t = malloc(sizeof(token));
	if (hexflag != 2)
		sscanf(numStr, "%lf", &number);
    else number = (int) strtol(numStr, NULL, 0);
    t->raw = numStr;
	t->numberValue = number;
	t->type = NUMBER;
	return t;
}

bool is_keyword(char* s) {
#define k(w) if (strcmp(s, w) == 0) return true;
	k("if")
#undef k
	return false;
}

token* _tkstr_read_ident(token_stream* s) {
	char* id = _tkstr_read_while(s, is_id);
	token* t = malloc(sizeof(token));
	t->raw = id;
	t->type = is_keyword(id) ? KEYWORD : VAR;
	return t;
}

token* _tkstr_read_string(token_stream* s) {
	token* t = malloc(sizeof(token));
	instr_next(s->instr);
	t->raw = _tkstr_read_escaped(s, '"');
	t->type = STRING;
	return t;
}

void _tkstr_skip_comment(token_stream* s) {
	bool is_not_newline(char c) {
		return c != '\n';
	}
	_tkstr_read_while(s, is_not_newline);
	instr_next(s->instr);
}


token* _tkstr_read_next(token_stream* s) {
	_tkstr_read_while(s, is_whitespace);
	sync_tkstr_fail(s);
	if (s->eof)
		return NULL;
	char c = instr_peek(s->instr);
	if (c == '\0') {
		instr_next(s->instr);
		sync_tkstr_fail(s);
		return NULL;
	} else if (c == '#') {
		_tkstr_skip_comment(s);
		return _tkstr_read_next(s);
	} else if (c == '"')
		return _tkstr_read_string(s);
	else if (is_digit(c))
		return _tkstr_read_number(s);
	else if (is_id_start(c))
		return _tkstr_read_ident(s);
	else if (is_punc(c)) {
		token* t = malloc(sizeof(token));
		t->raw = char2string(instr_next(s->instr));
		t->type = PUNC;
		return t;
	} else if (is_op_char(c)) {
		token* t = malloc(sizeof(token));
		t->raw = _tkstr_read_while(s, is_op_char);
		t->type = OP;
		return t;
	} else {
		char err[35] = {' '};
		sprintf(err, "Can't handle char (%d)", (int)c);
		tkstr_fail(s, err);
		return NULL;
	}		
}
